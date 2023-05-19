#ifndef _INCLUDE_HOOK_H
#define _INCLUDE_HOOK_H

#include <list>

#include "NextBotInterface.h"
#include "NextBotComponentInterface.h"
#include "NextBotIntentionInterface.h"

class INextBot;
using ActionTree = std::list<nb_action_ptr>;


class NextBotIntention : public IIntention
{
public:
	CBaseEntity* entity;
	nb_behavior_ptr behavior;

	inline nb_action_ptr GetAction();
};

class SurvivorBotIntention : public NextBotIntention
{
public:
	nb_behavior_ptr subehavior;

	inline nb_action_ptr GetSubAction();
};

class ComponentCollector
{
public:
	ComponentCollector(INextBot* bot)
	{
		for (INextBotComponent* i = bot->m_componentList; i != nullptr; i = i->m_nextComponent)
		{
			m_components.push_back(i);
		}
	}

	std::vector<INextBotComponent*> m_components;
};

void HookIntention(IIntention* intention);
void UnHookIntention(IIntention* intention);
void UnhookIntentions();

void CatchIntention(INextBot* bot, NextBotIntention* intention);
void UncatchCatchIntention(INextBot* bot, NextBotIntention* intention);

NextBotIntention* GetNextBotIntention(INextBotComponent* component);
INextBot* GetEntityNextbotPointer(CBaseEntity* entity);
bool GetEntityActions(CBaseEntity* entity, ActionTree& tree);

bool CreateActionsHook();
void DestroyActionsHook();

#endif // !_INCLUDE_HOOK_H
