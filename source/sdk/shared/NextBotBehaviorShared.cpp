#include "NextBotBehaviorShared.h"
#include "NextBotDebug.h"
#include "NextBotBehavior.h"

#undef clamp

#include "extension.h"
#include "actions_tools.h"

 bool behavior::IsDebuggingHistory()
{
	static ConVar* NextBotDebugHistory = icvar->FindVar("nb_debug_history");

	if (NextBotDebugHistory == nullptr)
		return false;

	return NextBotDebugHistory->GetBool();
}

void behavior::Debug(INextBot* me, nb_action_ptr action, nb_behavior_ptr behavior, const char* invokeStr)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	if (g_pActionsTools->IsDebugging(me, NEXTBOT_BEHAVIOR) || IsDebuggingHistory())
	{
		g_pActionsTools->DebugConColorMsg(me, NEXTBOT_BEHAVIOR, Color(255, 255, 150, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, me->GetDebugIdentifier(), behavior->GetName());
		g_pActionsTools->DebugConColorMsg(me, NEXTBOT_BEHAVIOR, Color(0, 255, 0, 255), invokeStr);
		g_pActionsTools->DebugConColorMsg(me, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), action->GetName());
		g_pActionsTools->DebugConColorMsg(me, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "\n");
	}
}

void behavior::Debug(CBaseEntity* me, nb_action_ptr action, nb_behavior_ptr behavior, const char* invokeStr)
{
	behavior::Debug(g_pActionsTools->MyNextBotPointer(me), action, behavior, invokeStr);
}

void behavior::ProcessEvent_DebugReceive(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	if (actor && (g_pActionsTools->IsDebugging(actor, NEXTBOT_EVENTS) || IsDebuggingHistory()))
	{
		INextBot* bot = g_pActionsTools->MyNextBotPointer(actor);

		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_EVENTS, Color(100, 100, 100, 255), "%3.2f: %s:%s: %s received EVENT %s\n", gpGlobals->curtime, bot->GetDebugIdentifier(), behavior->GetName(), action->GetFullName(), method);
	}
}

void behavior::ProcessEvent_DebugRespond(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method, const ActionResult<CBaseEntity>& result)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	if (actor && (g_pActionsTools->IsDebugging(actor, NEXTBOT_EVENTS) || IsDebuggingHistory()) && result.IsRequestingChange() && action)
	{
		INextBot* bot = g_pActionsTools->MyNextBotPointer(actor);

		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 0, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, bot->GetDebugIdentifier(), behavior->GetName());
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "%s ", action->GetFullName());
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 0, 255), "reponded to EVENT %s with ", method);
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 0, 0, 255), "%s %s ", result.GetTypeName(), result.m_action ? result.m_action->GetName() : "");
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(0, 255, 0, 255), "%s\n", result.m_reason ? result.m_reason : "");
	}
}

void behavior::ProcessEvent_DebugRespond(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method, const EventDesiredResult<CBaseEntity>& result)
{
	ActionResult<CBaseEntity> _result(result.m_type, result.m_action, result.m_reason);
	ProcessEvent_DebugRespond(actor, behavior, action, method, /* reinterpret_cast<const ActionResult<CBaseEntity>&>(result) */ _result);
}

void behavior::ApplyResult_Debug_ChangeTo(CBaseEntity* me,
	nb_action_ptr action, 
	nb_action_ptr newAction, 
	nb_behavior_ptr behavior, 
	const ActionResult<CBaseEntity>& result)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	INextBot* bot = g_pActionsTools->MyNextBotPointer(me);

	if (g_pActionsTools->IsDebugging(bot, NEXTBOT_BEHAVIOR) || behavior::IsDebuggingHistory())
	{
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 150, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, bot->GetDebugIdentifier(), behavior->GetName());

		if (action == newAction)
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 0, 0, 255), "START ");
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), newAction->GetName());
		}
		else
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), action->GetName());
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 0, 0, 255), " CHANGE_TO ");
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), newAction->GetName());
		}

		if (result.m_reason)
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(150, 255, 150, 255), "  (%s)\n", result.m_reason);
		}
		else
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "\n");
		}
	}
}

void behavior::ApplyResult_Debug_SuspendFor(CBaseEntity* me,
	nb_action_ptr action, 
	nb_action_ptr newAction, 
	nb_action_ptr topAction, 
	nb_behavior_ptr behavior, 
	const ActionResult<CBaseEntity>& result)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	INextBot* bot = g_pActionsTools->MyNextBotPointer(me);

	if (g_pActionsTools->IsDebugging(bot, NEXTBOT_BEHAVIOR) || behavior::IsDebuggingHistory())
	{
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 150, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, bot->GetDebugIdentifier(), behavior->GetName());

		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), action->GetName());
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 0, 255, 255), " caused ");
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), topAction->GetName());
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 0, 255, 255), " to SUSPEND_FOR ");
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), newAction->GetName());

		if (result.m_reason)
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(150, 255, 150, 255), "  (%s)\n", result.m_reason);
		}
		else
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "\n");
		}
	}
}

 void behavior::ApplyResult_Debug_Done(CBaseEntity* me,
	nb_action_ptr action, 
	nb_action_ptr newAction, 
	nb_action_ptr resumedAction, 
	nb_behavior_ptr behavior, 
	const ActionResult<CBaseEntity>& result)
{
	if (!g_pActionsTools->IsDebugSupported())
		return;

	INextBot* bot = g_pActionsTools->MyNextBotPointer(me);

	if (g_pActionsTools->IsDebugging(bot, NEXTBOT_BEHAVIOR) || behavior::IsDebuggingHistory())
	{
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 150, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, bot->GetDebugIdentifier(), behavior->GetName());
		g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), action->GetName());

		if (resumedAction)
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(0, 255, 0, 255), " DONE, RESUME ");
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), resumedAction->GetName());
		}
		else
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(0, 255, 0, 255), " DONE.");
		}

		if (result.m_reason)
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(150, 255, 150, 255), "  (%s)\n", result.m_reason);
		}
		else
		{
			g_pActionsTools->DebugConColorMsg(bot, NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "\n");
		}
	}
}

