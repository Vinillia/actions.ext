#pragma once

cell_t NAT_GetActionParent(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->m_parent;
}

cell_t NAT_GetActionChild(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActiveChildAction();
}

cell_t NAT_GetActionUnder(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActionBuriedUnderMe();
}

cell_t NAT_GetActionAbove(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActionCoveringMe();
}

cell_t NAT_GetActionActor(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	if (!action->m_isStarted)
		return -1;

	CBaseEntity* pEntity = static_cast<CBaseEntity*>(action->GetActor());
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t NAT_ActionSuspend(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	bool suspended = action->m_isSuspended;

	if (params[0] > 1)
	{
		action->m_isSuspended = static_cast<bool>(params[2]);
	}

	return suspended;
}

cell_t NAT_ActionStarted(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	bool started = action->m_isStarted;

	if (params[0] > 1)
	{
		action->m_isStarted = static_cast<bool>(params[2]);
	}

	return started;
}

cell_t NAT_GetActionName(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	const char* name = action->GetName();
	return pContext->StringToLocal(params[2], params[3], name);
}

cell_t NAT_GetEntityActions(IPluginContext* pContext, const cell_t* params)
{
	IPluginFunction* iterator = NULL;
	std::vector<Action<void>*> actions;
	size_t count = 0;

	iterator = pContext->GetFunctionById(params[2]);
	count = g_pActionsManager->GetEntityActions(params[1], iterator == NULL ? NULL : &actions);

	if (actions.size() == 0)
		return count;

	for (auto action : actions)
	{
		iterator->PushCell((cell_t)action);
		iterator->Execute(NULL);
	}

	return count;
}

cell_t NAT_ActionsAllocate(IPluginContext* pContext, const cell_t* params)
{
	return (cell_t)::operator new((size_t)params[1]);
}

cell_t NAT_ActionsDeallocate(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("You are trying to delete invalid action %X", action);
		return 0;
	}

	if (g_pActionsManager->GetRuntimeAction() == action)
		g_pActionsManager->SetRuntimeAction(NULL);

	delete action;
	return 0;
}

cell_t NAT_StorePendingEventResult(IPluginContext* pContext, const cell_t* params)
{
	Action<void>* action = (Action<void>*)params[1];

	if (!g_pActionsManager->IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<void> result;
	char* reason = NULL;

	pContext->LocalToStringNULL(params[4], &reason);

	result.m_type = (ActionResultType)params[2];
	result.m_action = (Action<void>*)params[3];
	result.m_reason = reason;
	result.m_priority = (EventResultPriorityType)params[5];

	action->StorePendingEventResult(result, NULL);
	return 0;
}

cell_t NAT_GetEntityAction(IPluginContext* pContext, const cell_t* params)
{
	std::vector<Action<void>*> actions; 
	cell_t entity;
	char* match;

	entity = params[1];
	pContext->LocalToString(params[2], &match);
	
	size_t num = g_pActionsManager->GetEntityActions(entity, &actions);

	if (num == 0)
		return 0;

	for(auto action : actions)
	{
		if (strcmp(action->GetName(), match) == 0)
			return (cell_t)action;
	}

	return 0;
}

sp_nativeinfo_t g_ActionNatives[] =
{
	{ "ActionsManager.Allocate", NAT_ActionsAllocate },
	{ "ActionsManager.Deallocate", NAT_ActionsDeallocate },
	{ "ActionsManager.Iterator", NAT_GetEntityActions },
	{ "ActionsManager.GetAction", NAT_GetEntityAction },

	{ "BehaviorAction.StorePendingEventResult", NAT_StorePendingEventResult },
	{ "BehaviorAction.GetName", NAT_GetActionName },

	{ "BehaviorAction.Parent.get", NAT_GetActionParent },
	{ "BehaviorAction.Child.get", NAT_GetActionChild },
	{ "BehaviorAction.Under.get", NAT_GetActionUnder },
	{ "BehaviorAction.Above.get", NAT_GetActionAbove },
	{ "BehaviorAction.Actor.get", NAT_GetActionActor },

	{ "BehaviorAction.IsSuspended.get", NAT_ActionSuspend },
	{ "BehaviorAction.IsSuspended.set", NAT_ActionSuspend },

	{ "BehaviorAction.IsStarted.get", NAT_ActionStarted },
	{ "BehaviorAction.IsStarted.set", NAT_ActionStarted },

	{ NULL, NULL }
};