#pragma once

#ifndef _INCLUDE_ACTIONS_NATIVES_H
#define _INCLUDE_ACTIONS_NATIVES_H

static inline  ActionResult< CBaseEntity > Continue(void)
{
	return ActionResult< CBaseEntity >(CONTINUE, NULL, NULL);
}

static inline  ActionResult< CBaseEntity > ChangeTo(Action< CBaseEntity >* action, const char* reason)
{
	return ActionResult< CBaseEntity >(CHANGE_TO, action, reason);
}

static inline  ActionResult< CBaseEntity > SuspendFor(Action< CBaseEntity >* action, const char* reason)
{
	return ActionResult< CBaseEntity >(SUSPEND_FOR, action, reason);
}

static inline  ActionResult< CBaseEntity > Done(const char* reason)
{
	return ActionResult< CBaseEntity >(DONE, NULL, reason);
}

static inline  EventDesiredResult< CBaseEntity > TryContinue(EventResultPriorityType priority)
{
	return EventDesiredResult< CBaseEntity >(CONTINUE, NULL, priority);
}

static inline  EventDesiredResult< CBaseEntity > TryChangeTo(Action< CBaseEntity >* action, EventResultPriorityType priority, const char* reason)
{
	return EventDesiredResult< CBaseEntity >(CHANGE_TO, action, priority, reason);
}

static inline  EventDesiredResult< CBaseEntity > TrySuspendFor(Action< CBaseEntity >* action, EventResultPriorityType priority, const char* reason)
{
	return EventDesiredResult< CBaseEntity >(SUSPEND_FOR, action, priority, reason);
}

static inline  EventDesiredResult< CBaseEntity > TryDone(EventResultPriorityType priority, const char* reason = NULL)
{
	return EventDesiredResult< CBaseEntity >(DONE, NULL, priority, reason);
}

static inline EventDesiredResult< CBaseEntity > TryToSustain(EventResultPriorityType priority, const char* reason = NULL)
{
	return EventDesiredResult< CBaseEntity >(SUSTAIN, NULL, priority, reason);
}

inline bool _action_changelistener(IPluginContext* ctx, const cell_t* params, bool remove)
{
	nb_action_ptr action = (nb_action_ptr)params[1];
	HashValue hash = (HashValue)params[2];
	IPluginFunction* fn = ctx->GetFunctionById((funcid_t)params[3]);
	bool post = (bool)params[4];

	ActionPropagation* propagation = (post ? &g_actionsPropagationPost : &g_actionsPropagationPre);

	if (fn == nullptr)
	{
		return propagation->RemoveListener(action, hash, ctx);
	}

	if (remove)
	{
		return propagation->RemoveListener(action, hash, fn);
	}

	return propagation->AddListener(action, hash, fn);
}

cell_t NAT_actions_setlistener(IPluginContext* ctx, const cell_t* params)
{
	return (cell_t)_action_changelistener(ctx, params, false);
}

cell_t NAT_actions_removelistener(IPluginContext* ctx, const cell_t* params)
{
	return (cell_t)_action_changelistener(ctx, params, true);
}

cell_t NAT_actions_GetName(IPluginContext* ctx, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		ctx->ReportError("Invalid action passed %X", action);
		return 0;
	}

	const char* name = action->GetName();
	return ctx->StringToLocal(params[2], params[3], name);
}

cell_t NAT_actions_GetParent(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->m_parent;
}

cell_t NAT_actions_GetChild(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActiveChildAction();
}

cell_t NAT_actions_GetUnder(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActionBuriedUnderMe();
}

cell_t NAT_actions_GetAbove(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return (cell_t)action->GetActionCoveringMe();
}

cell_t NAT_actions_GetActor(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	return gamehelpers->EntityToBCompatRef(g_actionsManager.GetActionActor(action));
}

cell_t NAT_actions_IsSuspended(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
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

cell_t NAT_actions_IsStarted(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
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

cell_t NAT_actions_GetReason(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<CBaseEntity>* actionResult = (ActionResult<CBaseEntity>*)params[1];
	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	if (runtimeResult != actionResult)
	{
		pContext->ReportError("Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!", runtimeResult, actionResult);
		return -1;
	}

	if (actionResult->m_reason == NULL)
		return -1;

	return pContext->StringToLocal(params[2], params[3], actionResult->m_reason);
}

cell_t NAT_actions_SetReason(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<CBaseEntity>* actionResult = (ActionResult<CBaseEntity>*)params[1];
	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result. Make sure function callback matches prototype!");
		return -1;
	}

	if (runtimeResult != actionResult)
	{
		pContext->ReportError("Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!", runtimeResult, actionResult);
		return -1;
	}

	char* reason;
	pContext->LocalToString(params[2], &reason);

	actionResult->m_reason = reason;
	return 1;
}

cell_t NAT_actions_SetOrGetType(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<CBaseEntity>* actionResult = (ActionResult<CBaseEntity>*)params[1];
	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	if (runtimeResult != actionResult)
	{
		pContext->ReportError("Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!", runtimeResult, actionResult);
		return -1;
	}

	ActionResultType result = actionResult->m_type;

	if (params[0] > 1)
		actionResult->m_type = (ActionResultType)params[2];

	return result;
}

cell_t NAT_actions_SetOrGetAction(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<CBaseEntity>* actionResult = (ActionResult<CBaseEntity>*)params[1];
	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	if (runtimeResult != actionResult)
	{
		pContext->ReportError("Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!", runtimeResult, actionResult);
		return -1;
	}

	nb_action_ptr action = actionResult->m_action;

	if (params[0] > 1)
	{
		actionResult->m_action = (nb_action_ptr)params[2];
	}

	return (cell_t)action;
}

cell_t NAT_actions_SetOrGetPriority(IPluginContext* pContext, const cell_t* params)
{
	EventDesiredResult<CBaseEntity>* actionResult = (EventDesiredResult<CBaseEntity>*)params[1];
	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	if (runtimeResult != actionResult)
	{
		pContext->ReportError("Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!", runtimeResult, actionResult);
		return -1;
	}

	EventResultPriorityType result = actionResult->m_priority;

	if (params[0] > 1)
	{
		actionResult->m_priority = (EventResultPriorityType)params[2];
	}

	return (cell_t)result;
}

cell_t NAT_actions_Continue(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	*runtimeResult = Continue();
	return Pl_Changed;
}

cell_t NAT_actions_ChangeTo(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[3], &reason);

	*runtimeResult = ChangeTo((nb_action_ptr)(params[2]), reason);
	return Pl_Changed;
}

cell_t NAT_actions_SuspendFor(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[3], &reason);

	nb_action_ptr runtimeAction = g_actionsManager.GetRuntimeAction();

	if (runtimeAction)
		runtimeAction->m_eventResult = TryContinue(RESULT_NONE);

	*runtimeResult = SuspendFor((nb_action_ptr)(params[2]), reason);
	return Pl_Changed;
}

cell_t NAT_actions_Done(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	ActionResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[2], &reason);

	*runtimeResult = Done(reason);
	return Pl_Changed;
}

cell_t NAT_actions_TryContinue(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	*runtimeResult = TryContinue((EventResultPriorityType)params[2]);
	return Pl_Changed;
}

cell_t NAT_actions_TryChangeTo(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[4], &reason);

	*runtimeResult = TryChangeTo((nb_action_ptr)params[2], (EventResultPriorityType)params[3], reason);
	return Pl_Changed;
}

cell_t NAT_actions_TrySuspendFor(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[4], &reason);

	*runtimeResult = TrySuspendFor((nb_action_ptr)(params[2]), (EventResultPriorityType)params[3], reason);
	return Pl_Changed;
}

cell_t NAT_actions_TryDone(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[3], &reason);

	*runtimeResult = TryDone((EventResultPriorityType)params[2], reason);
	return Pl_Changed;
}

cell_t NAT_actions_TryToSustain(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	EventDesiredResult<CBaseEntity>* runtimeResult = g_actionsManager.GetActionRuntimeDesiredResult();

	if (!runtimeResult)
	{
		pContext->ReportError("Attempt to access an invalid result.");
		return -1;
	}

	char* reason;
	pContext->LocalToStringNULL(params[3], &reason);

	*runtimeResult = TryToSustain((EventResultPriorityType)params[2], reason);
	return Pl_Changed;
}

cell_t NAT_actions_StorePendingEventResult(IPluginContext* pContext, const cell_t* params)
{
	return 0;
}

cell_t NAT_actions_GetActionUserData(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	char* str;
	cell_t* pData;

	pContext->LocalToString(params[2], &str);
	pContext->LocalToPhysAddr(params[3], &pData);

	void* data;
	if (g_actionsManager.GetUserData(action, str, data))
	{
		*pData = (cell_t)data;
		return 1;
	}

	return 0;
}

cell_t NAT_actions_SetActionUserData(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	char* str;
	void* data = nullptr;

	pContext->LocalToString(params[2], &str);
	data = (void*)params[3];

	g_actionsManager.SetUserData(action, str, data);
	return 0;
}

cell_t NAT_actions_GetActionUserDataIdentity(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];
	char* str;
	cell_t* pData;

	pContext->LocalToString(params[2], &str);
	pContext->LocalToPhysAddr(params[3], &pData);

	void* data;
	if (g_actionsManager.GetUserDataIdentity(action, { pContext->GetIdentity(), str }, data))
	{
		*pData = (cell_t)data;
		return 1;
	}

	return 0;
}

cell_t NAT_actions_SetActionUserDataIdentity(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = (nb_action_ptr)params[1];

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	char* str;
	void* data = nullptr;

	pContext->LocalToString(params[2], &str);
	data = (void*)params[3];

	g_actionsManager.SetUserDataIdentity(action, { pContext->GetIdentity(), str }, data);
	return 0;
}

cell_t NAT_actions_CreateComponent(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[1]);
	IPluginFunction* fnInitial = pContext->GetFunctionById(params[2]);
	IPluginFunction* fnUpdate = pContext->GetFunctionById(params[3]);
	IPluginFunction* fnUpkeep = pContext->GetFunctionById(params[4]);
	IPluginFunction* fnReset = pContext->GetFunctionById(params[5]);
	char* compName = nullptr;
	INextBot* nextbot = nullptr;

	if (entity == nullptr)
	{
		pContext->ReportError("Invalid entity index %i", params[1]);
		return 0;
	}

	pContext->LocalToStringNULL(params[6], &compName);
	nextbot = g_pActionsTools->MyNextBotPointer(entity);

	if (nextbot == nullptr)
	{
		pContext->ReportError("Failed to get entity nextbot ptr %i", params[1]);
		return 0;
	}
 
	ActionComponent* component = new ActionComponent(nextbot, pContext, fnInitial, compName);

	if (component->HasHandleError())
	{
		pContext->ReportError("Failed to create handle (error %i)", component->GetHandleError());
		return 0;
	}

	component->SetUpdateCallback(fnUpdate);
	component->SetUpkeepCallback(fnUpkeep);
	component->SetResetCallback(fnReset);

	return component->GetHandle();
}

cell_t NAT_actions_ComponentUpdateCallback(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	IPluginFunction* fn = pContext->GetFunctionById(params[2]);

	if (fn == nullptr)
	{
		pContext->ReportError("Invalid function %i", params[2]);
		return 0;
	}

	component->SetUpdateCallback(fn);
	return 0;
}

cell_t NAT_actions_ComponentAddress(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	return (cell_t)component;
}

cell_t NAT_actions_ComponentUpkeepCallback(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	IPluginFunction* fn = pContext->GetFunctionById(params[2]);

	if (fn == nullptr)
	{
		pContext->ReportError("Invalid function %i", params[2]);
		return 0;
	}

	component->SetUpkeepCallback(fn);
	return 0;
}

cell_t NAT_actions_ComponentResetCallback(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return pContext->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	IPluginFunction* fn = pContext->GetFunctionById(params[2]);

	if (fn == nullptr)
	{
		pContext->ReportError("Invalid function %i", params[2]);
		return 0;
	}

	component->SetResetCallback(fn);
	return 0;
}

cell_t NAT_actions_ComponentActor(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		pContext->ReportError("Invalid component handle %x (error %d)", hndl, err);
		return 0;
	}

	INextBot* bot = component->GetBot();

	if (bot == nullptr)
		return -1;

	CBaseEntity* entity = g_pActionsTools->GetEntity(bot);

	if (entity == nullptr)
	{
		pContext->ReportError("Failed to get nextbot ptr (error %X)", entity);
		return 0;
	}

	int entindex = gamehelpers->EntityToBCompatRef(entity);

	if (entindex == -1)
		return 0;

	return entindex;
}

/*
cell_t NAT_actions_ActorComponents(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[1]);

	if (entity == nullptr)
	{
		pContext->ReportError("Invalid entity index %i", params[1]);
		return 0;
	}

	INextBot* bot = g_pActionsTools->MyNextBotPointer(entity);

	if (bot == nullptr)
	{
		pContext->ReportError("Failed to get entity nextbot ptr %i", params[1]);
		return 0;
	}

	return 0;
}*/

cell_t NAT_actions_ComponentGetName(IPluginContext* ctx, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]); 
	HandleSecurity sec(nullptr, myself->GetIdentity());
	HandleError err;

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return ctx->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	const char* name = component->GetName();

	if (name == nullptr)
	{
		ctx->StringToLocal(params[2], params[3], "null");
		return -1;
	}

	return ctx->StringToLocal(params[2], params[3], name);
}

cell_t NAT_actions_ComponentSetName(IPluginContext* ctx, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError err;
	HandleSecurity sec;

	sec.pOwner = ctx->GetIdentity();
	sec.pIdentity = myself->GetIdentity();

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return ctx->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	char* name;
	ctx->LocalToString(params[2], &name);
	component->SetName(name);
	return 0;
}

cell_t NAT_actions_ComponentCurrentAction(IPluginContext* ctx, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError err;
	HandleSecurity sec;

	sec.pOwner = ctx->GetIdentity();
	sec.pIdentity = myself->GetIdentity();

	ActionComponent* component = nullptr;
	if ((err = g_pHandleSys->ReadHandle(hndl, g_sdkActions.GetComponentHT(), &sec, (void**)&component))
		!= HandleError_None)
	{
		return ctx->ThrowNativeError("Invalid component handle %x (error %d)", hndl, err);
	}

	return (cell_t)component->CurrentAction();
}

sp_nativeinfo_t g_actionsNatives[] =
{
	{ "__action_setlistener",						NAT_actions_setlistener },
	{ "__action_removelistener",					NAT_actions_removelistener },

	{ "ActionsManager.SetActionUserDataIdentity",	NAT_actions_SetActionUserDataIdentity },
	{ "ActionsManager.GetActionUserDataIdentity",	NAT_actions_GetActionUserDataIdentity },
	{ "ActionsManager.SetActionUserData",			NAT_actions_SetActionUserData },
	{ "ActionsManager.GetActionUserData",			NAT_actions_GetActionUserData },

	{ "ActionComponent.ActionComponent",			NAT_actions_CreateComponent },
	{ "ActionComponent.Address",					NAT_actions_ComponentAddress },
	{ "ActionComponent.Actor",						NAT_actions_ComponentActor },
	{ "ActionComponent.GetName",					NAT_actions_ComponentGetName },
	{ "ActionComponent.SetName",					NAT_actions_ComponentSetName },
	{ "ActionComponent.CurrentAction.get",			NAT_actions_ComponentCurrentAction },
	{ "ActionComponent.Update.set",					NAT_actions_ComponentUpdateCallback },
	{ "ActionComponent.Upkeep.set",					NAT_actions_ComponentUpkeepCallback },
	{ "ActionComponent.Reset.set",					NAT_actions_ComponentResetCallback },

	{ "BehaviorAction.GetName",						NAT_actions_GetName },

	{ "BehaviorAction.StorePendingEventResult",		NAT_actions_StorePendingEventResult },
	{ "BehaviorAction.Continue",					NAT_actions_Continue },
	{ "BehaviorAction.ChangeTo",					NAT_actions_ChangeTo },
	{ "BehaviorAction.SuspendFor",					NAT_actions_SuspendFor },
	{ "BehaviorAction.Done",						NAT_actions_Done },
	{ "BehaviorAction.TryContinue",					NAT_actions_TryContinue },
	{ "BehaviorAction.TryChangeTo",					NAT_actions_TryChangeTo },
	{ "BehaviorAction.TrySuspendFor",				NAT_actions_TrySuspendFor },
	{ "BehaviorAction.TryDone",						NAT_actions_TryDone },
	{ "BehaviorAction.TryToSustain",				NAT_actions_TryToSustain },

	{ "BehaviorAction.Parent.get",					NAT_actions_GetParent },
	{ "BehaviorAction.Child.get",					NAT_actions_GetChild },
	{ "BehaviorAction.Under.get",					NAT_actions_GetUnder },
	{ "BehaviorAction.Above.get",					NAT_actions_GetAbove },
	{ "BehaviorAction.Actor.get",					NAT_actions_GetActor },

	{ "BehaviorAction.IsSuspended.get",				NAT_actions_IsSuspended },
	{ "BehaviorAction.IsSuspended.set",				NAT_actions_IsSuspended },

	{ "BehaviorAction.IsStarted.get",				NAT_actions_IsStarted },
	{ "BehaviorAction.IsStarted.set",				NAT_actions_IsStarted },

	{ "ActionResult.GetReason",						NAT_actions_GetReason },
	{ "ActionResult.SetReason",						NAT_actions_SetReason },

	{ "ActionResult.type.set",						NAT_actions_SetOrGetType },
	{ "ActionResult.type.get",						NAT_actions_SetOrGetType },

	{ "ActionResult.action.set",					NAT_actions_SetOrGetAction },
	{ "ActionResult.action.get",					NAT_actions_SetOrGetAction },

	{ "ActionDesiredResult.priority.set",			NAT_actions_SetOrGetPriority },
	{ "ActionDesiredResult.priority.get",			NAT_actions_SetOrGetPriority },

	{ NULL, NULL }
};

#endif // !_INCLUDE_ACTIONS_NATIVES_H
