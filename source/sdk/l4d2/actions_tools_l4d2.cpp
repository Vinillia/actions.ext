#include "actionsdefs.h"
#include "actions_tools_l4d2.h"
#include "actions_manager.h"

#include "NextBotIntentionInterface.h"
#include "NextBotBehavior.h"

#if SOURCE_ENGINE != SE_LEFT4DEAD
L4D2ActionsTools g_L4D2ActionTools;
#endif

L4D2ActionsTools::L4D2ActionsTools()
{
	m_survivorBotPointerOffs = -1;
}

bool L4D2ActionsTools::OnIntentionReset(INextBot* bot, IIntention* intention)
{
	if (MySurvivorBotPointer(bot))
	{
		nb_action_ptr action = ActionNextContainedResponder(intention);

		if (action == nullptr)
			return false;

		g_actionsManager.Add(action, GetEntity(bot));
	}

	return ActionsTools::OnIntentionReset(bot, intention);
}

void L4D2ActionsTools::GetIntentionActions(IIntention* intention, std::vector<nb_action_ptr>& vec)
{
	if (MySurvivorBotPointer(intention->GetBot()))
	{
		nb_action_ptr action = ActionNextContainedResponder(intention);

		if (action)
			m_insertAction(action, vec);
	}

	ActionsTools::GetIntentionActions(intention, vec);
}

bool L4D2ActionsTools::LoadGameConfigFile(IGameConfig* config, char* error, size_t maxlen)
{
	if (!config->GetOffset("MySurvivorBotPointer", &m_survivorBotPointerOffs))
	{
		V_snprintf(error, maxlen, "Failed to find MySurvivorBotPointer offset");
		return false;
	}

	return ActionsTools::LoadGameConfigFile(config, error, maxlen);
}

nb_action_ptr L4D2ActionsTools::ActionNextContainedResponder(IIntention* intention)
{
	nb_behavior_ptr behavior = (nb_behavior_ptr)(intention->NextContainedResponder(intention->FirstContainedResponder()));

	if (behavior == nullptr)
		return nullptr;

	return (nb_action_ptr)behavior->FirstContainedResponder();
}
