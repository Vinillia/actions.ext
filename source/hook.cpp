#include <CDetour/detours.h>
#include "actionsdefs.h"
#include "actions_manager.h"
#include "actions_component.h"
#include "hook.h"

#include "sourcehook.h"

#include <NextBotInterface.h>
#include <NextBotComponentInterface.h>
#include <NextBotBehavior.h>
#include <NextBotIntentionInterface.h>

#include <unordered_map>
#include <functional>

#define FOR_EACH_COMPONENT(bot,i) for( INextBotComponent* i = bot->m_componentList; i != nullptr; i = i->m_nextComponent )

std::unordered_map<CBaseEntity*, INextBot*> g_entitiesNextbot;
std::vector<IIntention*> g_vecHookedIntentions(32);
CDetour* g_pNextBotResetDetour = nullptr;

SH_DECL_HOOK0_void(IIntention, Reset, SH_NOATTRIB, 0);

inline nb_action_ptr NextBotIntention::GetAction()
{
	if (behavior == nullptr)
		return nullptr;

	return (nb_action_ptr)(behavior->FirstContainedResponder());
}

inline nb_action_ptr SurvivorBotIntention::GetSubAction()
{
	if (subehavior == nullptr)
		return nullptr;

	return (nb_action_ptr)(subehavior->FirstContainedResponder());
}

void IIntention__ResetPost()
{
	IIntention* intention = META_IFACEPTR(IIntention);
	CatchIntention(intention->GetBot(), (NextBotIntention*)intention);
}

DETOUR_DECL_MEMBER0(INextBot__Reset, void)
{
	INextBot* bot = (INextBot*)(this);

	DETOUR_MEMBER_CALL(INextBot__Reset)();

	FOR_EACH_COMPONENT(bot, component)
	{
		NextBotIntention* intention = GetNextBotIntention(component);

		if (!intention)
			continue;

		CatchIntention(bot, intention);
		HookIntention(intention);
	}
}

void CatchIntention(INextBot* bot, NextBotIntention* intention)
{
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

void UnhookIntentions()
{
	for (auto intention : g_vecHookedIntentions)
	{
		SH_REMOVE_HOOK(IIntention, Reset, intention, IIntention__ResetPost, true);
	}

	g_vecHookedIntentions.clear();
}

void HookIntention(IIntention* intention)
{
	SH_ADD_HOOK(IIntention, Reset, intention, IIntention__ResetPost, true);
	g_vecHookedIntentions.push_back(intention);
}

void UnHookIntention(IIntention* intention)
{
	SH_REMOVE_HOOK(IIntention, Reset, intention, IIntention__ResetPost, true);
	g_vecHookedIntentions.erase(std::find(g_vecHookedIntentions.begin(), g_vecHookedIntentions.end(), intention));
}


bool CreateActionsHook()
{
	g_pNextBotResetDetour = DETOUR_CREATE_MEMBER(INextBot__Reset, "INextBot::Reset");

	if (g_pNextBotResetDetour)
	{
		g_pNextBotResetDetour->EnableDetour();
		return true;
	}

	return false;
}

void DestroyActionsHook()
{
	if (g_pNextBotResetDetour)
	{
		g_pNextBotResetDetour->Destroy();
		g_pNextBotResetDetour = nullptr;
	}

	UnhookIntentions();
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