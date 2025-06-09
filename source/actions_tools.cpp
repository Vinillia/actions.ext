#include "actionsdefs.h"
#include "actions_tools.h"
#include "actions_manager.h"

#include "NextBotIntentionInterface.h"
#include "NextBotBehavior.h"
#include "NextBotDebug.h"

#include <Color.h>
#include <am-function.h>

ActionsTools* g_pActionsTools = nullptr;

ActionsTools::ActionsTools()
{
	AssertFatalMsg(g_pActionsTools == nullptr, "Only one instance ActionTools is allowed!");
	g_pActionsTools = this;

	m_insertAction = [this](nb_action_ptr head, std::vector<nb_action_ptr>& vec)
	{
		vec.push_back(head);

		if (head->GetActionBuriedUnderMe())
			m_insertAction(head->GetActionBuriedUnderMe(), vec);

		if (head->GetActiveChildAction())
			m_insertAction(head->GetActiveChildAction(), vec);
	};
	
	m_NextBot_GetEntity_Offset = -1;
	m_Entity_MyNextBotPointer_Offset = -1;
	m_NextBot_DebugConColorMsg_Address = nullptr;
	m_NextBot_IsDebugging_Address = nullptr;
}

bool ActionsTools::OnIntentionReset(INextBot* bot, IIntention* intention)
{
	nb_action_ptr action = ActionContainedResponder(intention);

	if (action == nullptr)
		return false;

	g_actionsManager.Add(action, GetEntity(bot));
	return true;
}

bool ActionsTools::OnNextBotReset(INextBot* bot)
{
	for (INextBotComponent* component = bot->m_componentList; component != nullptr; component = component->m_nextComponent)
	{
		IIntention* intention = TryCastToIntentionComponent(component);

		if (intention == nullptr)
			continue;

		OnIntentionReset(bot, intention);
	}

	return true;
}

void ActionsTools::GetIntentionActions(IIntention* intention, std::vector<nb_action_ptr>& vec)
{
	nb_action_ptr head = ActionContainedResponder(intention);

	if (head)
	{
		m_insertAction(head, vec);
	}
}

bool ActionsTools::GetEntityActions(CBaseEntity* entity, std::vector<nb_action_ptr>& vec)
{
	INextBot* nextbot = MyNextBotPointer(entity);

	if (nextbot == nullptr)
		return false;

	for (INextBotComponent* comp = nextbot->m_componentList; comp != nullptr; comp = comp->m_nextComponent)
	{
		IIntention* intention = TryCastToIntentionComponent(comp);

		if (intention == nullptr)
			continue;

		GetIntentionActions(intention, vec);
	}

	return true;
}

IIntention* ActionsTools::TryCastToIntentionComponent(INextBotComponent* component)
{
	return dynamic_cast<IIntention*>(component);
}

bool ActionsTools::LoadGameConfigFile(IGameConfig* config, char* error, size_t maxlen)
{
	if (!config->GetOffset("GetEntity", &m_NextBot_GetEntity_Offset))
	{
		V_snprintf(error, maxlen, "Failed to find INextBot::GetEntity offset");
		return false;
	}
	
	if (!config->GetOffset("MyNextBotPointer", &m_Entity_MyNextBotPointer_Offset))
	{
		V_snprintf(error, maxlen, "Failed to find CBaseEntity::MyNextBotPointer offset");
		return false;
	}
	
	auto Config_GetMemOrSig = [config](const char* key, void** addr) -> bool
	{
		if (!config->GetMemSig(key, addr) &&
			!config->GetAddress(key, addr))
		{
			return false;
		}

		return (*addr) != nullptr;
	};

	if (!Config_GetMemOrSig("INextBot::DebugConColorMsg", &m_NextBot_DebugConColorMsg_Address))
	{
#if __DEBUG
		MsgSM("Debug disabled: Failed to find INextBot::DebugConColorMsg");
#endif // __DEBUG
	}

	if (!Config_GetMemOrSig("INextBot::IsDebugging", &m_NextBot_IsDebugging_Address))
	{
#if __DEBUG
		MsgSM("Debug disabled: Failed to find INextBot::IsDebugging");
#endif // __DEBUG
	}
	
	return true;
}

nb_action_ptr ActionsTools::ActionContainedResponder(IIntention* intention)
{
	nb_behavior_ptr behavior = (nb_behavior_ptr)intention->FirstContainedResponder();

	if (behavior == nullptr)
		return nullptr;

	return (nb_action_ptr)behavior->FirstContainedResponder();
}

