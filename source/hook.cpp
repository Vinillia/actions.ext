#include <CDetour/detours.h>
#include "actionsdefs.h"
#include "actions_manager.h"
#include "actions_component.h"

#include <NextBotInterface.h>
#include <NextBotComponentInterface.h>
#include <NextBotBehavior.h>
#include <NextBotIntentionInterface.h>

#include <unordered_map>

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
extern CDetour* g_pDestructorLock;

DETOUR_DECL_MEMBER0(INextBot__Reset, void)
{
	INextBot* bot = (INextBot*)(this);

	DETOUR_MEMBER_CALL(INextBot__Reset)();

	auto component = bot->m_componentList;
	while (component)
	{
		IIntention* intention = dynamic_cast<IIntention*>(component);

		if (intention)
		{
			NextBotIntention* i = static_cast<NextBotIntention*>(intention);
			
			if (bot->GetEntity())
				g_entitiesNextbot[(CBaseEntity*)bot->GetEntity()] = bot;

			if (i->GetAction())
			{
				g_actionsManager.SetRuntimeActor(i->entity);
				g_actionsManager.Add(i->GetAction());
			}

			if (bot->MySurvivorBotPointer())
			{
				g_actionsManager.SetRuntimeActor(i->entity);
				nb_action_ptr action = ((SurvivorBotIntention*)(i))->GetSubAction();

				if (action)
					g_actionsManager.Add(action);
			}
		}

		component = component->m_nextComponent;
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

	if (g_pDestructorLock)
	{
		g_pDestructorLock->Destroy();
		g_pDestructorLock = nullptr;
	}
}

INextBot* GetEntityNextbotPointer(CBaseEntity* entity)
{
	auto r = g_entitiesNextbot.find(entity);

	if (r == g_entitiesNextbot.cend())
		return nullptr;

	return r->second;
}