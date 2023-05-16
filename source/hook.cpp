#include <CDetour/detours.h>
#include "actionsdefs.h"
#include "actions_manager.h"
#include "actions_component.h"
#include "hook.h"

#include <NextBotInterface.h>
#include <NextBotComponentInterface.h>
#include <NextBotBehavior.h>
#include <NextBotIntentionInterface.h>

#include <unordered_map>
#include <functional>

#define FOR_EACH_COMPONENT(bot,i) for( INextBotComponent* i = bot->m_componentList; i != nullptr; i = i->m_nextComponent )

class NextBotIntention : public IIntention
{
public:
	CBaseEntity* entity;
	nb_behavior_ptr behavior;

	inline nb_action_ptr GetAction()
	{
		if (behavior == nullptr)
			return nullptr;

		return (nb_action_ptr)(behavior->FirstContainedResponder());
	}
};

class SurvivorBotIntention : public NextBotIntention
{
public:
	nb_behavior_ptr subehavior;

	inline nb_action_ptr GetSubAction()
	{
		if (subehavior == nullptr)
			return nullptr;

		return (nb_action_ptr)(subehavior->FirstContainedResponder());
	}
};

std::unordered_map<CBaseEntity*, INextBot*> g_entitiesNextbot;
CDetour* g_pNextBotDetour = nullptr;

NextBotIntention* GetNextBotIntention(INextBotComponent* component);

DETOUR_DECL_MEMBER0(INextBot__Reset, void)
{
	INextBot* bot = (INextBot*)(this);

	DETOUR_MEMBER_CALL(INextBot__Reset)();

	FOR_EACH_COMPONENT(bot, component)
	{
		NextBotIntention* intention = GetNextBotIntention(component);

		if (!intention)
			continue;

		nb_action_ptr action = intention->GetAction();

		if (bot->GetEntity())
		{
			g_entitiesNextbot[intention->entity] = bot;
		}

		if (action)
		{
			g_actionsManager.SetActionActor(action, intention->entity);
			g_actionsManager.Add(action);
		}

		if (bot->MySurvivorBotPointer())
		{
			nb_action_ptr subaction = ((SurvivorBotIntention*)(intention))->GetSubAction();

			if (subaction)
			{
				g_actionsManager.SetActionActor(subaction, intention->entity);
				g_actionsManager.Add(subaction);
			}
		}
	}
}

bool CreateActionsHook()
{
	g_pNextBotDetour = DETOUR_CREATE_MEMBER(INextBot__Reset, "INextBot::Reset");

	if (g_pNextBotDetour)
	{
		g_pNextBotDetour->EnableDetour();
		return true;
	}

	return false;
}

void DestroyActionsHook()
{
	if (g_pNextBotDetour)
	{
		g_pNextBotDetour->Destroy();
		g_pNextBotDetour = nullptr;
	}
}

NextBotIntention* GetNextBotIntention(INextBotComponent* component)
{
	IIntention* intention = dynamic_cast<IIntention*>(component);

	if (!intention)
		return nullptr;

	return static_cast<NextBotIntention*>(intention);
}

INextBot* GetEntityNextbotPointer(CBaseEntity* entity)
{
	auto r = g_entitiesNextbot.find(entity);

	if (r == g_entitiesNextbot.cend())
		return nullptr;

	return r->second;
}

bool GetEntityActions(CBaseEntity* entity, ActionTree& tree)
{
	INextBot* nextbot = GetEntityNextbotPointer(entity);

	if (nextbot == nullptr)
		return false;

	std::function<void(nb_action_ptr)> emplace_back = [&emplace_back, &tree](nb_action_ptr head)
	{
		tree.emplace_back(head);

		if (head->GetActionBuriedUnderMe())
			emplace_back(head->GetActionBuriedUnderMe());

		if (head->GetActiveChildAction())
			emplace_back(head->GetActiveChildAction());
	};

	FOR_EACH_COMPONENT(nextbot, comp)
	{
		NextBotIntention* intention = GetNextBotIntention(comp);

		if (intention == nullptr)
			continue;

		nb_action_ptr head = intention->GetAction();

		if (head == nullptr)
			continue;

		emplace_back(head);

		if (nextbot->MySurvivorBotPointer())
		{
			head = ((SurvivorBotIntention*)(intention))->GetSubAction();

			if (head)
				emplace_back(head);
		}
	}

	return true;
}