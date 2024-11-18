#pragma once

#include <actions_custom_legacy.h>

cell_t NAT_action_Allocate(IPluginContext* pContext, const cell_t* params)
{
	return ToPseudoAddress((void*)::operator new((size_t)params[1]));
}

cell_t NAT_action_Deallocate(IPluginContext* pContext, const cell_t* params)
{
	nb_action_ptr action = get_native_param<nb_action_ptr>(params, 1);

	if (!g_actionsManager.IsValidAction(action))
	{
		pContext->ReportError("You are trying to delete invalid action %X", action);
		return 0;
	}

	delete action;
	return 0;
}

cell_t NAT_action_GetEntityActions(IPluginContext* ctx, const cell_t* params)
{
	IPluginFunction* fn = ctx->GetFunctionById(params[2]);
	
	if (fn == nullptr)
	{
		ctx->ReportError("Invalid function iterator %i", params[2]);
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[1]);
	std::vector<nb_action_ptr> actions;

	if (entity == nullptr || !g_pActionsTools->GetEntityActions(entity, actions))
	{
		ctx->ReportError("Invalid entity index %i", params[1]);
		return 0;
	}

	for (auto action : actions)
	{
		fn->PushCell(ToPseudoAddress(action));
		fn->Execute(NULL);
	}

	return static_cast<cell_t>(actions.size());
}

cell_t NAT_action_GetEntityAction(IPluginContext* ctx, const cell_t* params)
{
	std::vector<nb_action_ptr> actions;
	char* match;

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[1]);
	ctx->LocalToString(params[2], &match);

	if (entity == nullptr || !g_pActionsTools->GetEntityActions(entity, actions))
	{
		ctx->ReportError("Invalid entity index %i", params[1]);
		return 0;
	}

	auto res = std::find_if(actions.cbegin(), actions.cend(), [match](nb_action_ptr action) { return (strcmp(action->GetName(), match) == 0); });

	if (res == actions.cend())
		return 0;

	return ToPseudoAddress(*res);
}

cell_t NAT_action_Create(IPluginContext* pContext, const cell_t* params)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	ActionCustomLegacy* action = new ActionCustomLegacy((const char*)name);
	g_actionsManager.AddPending((nb_action_ptr)action);

	return ToPseudoAddress(action);
}

sp_nativeinfo_t g_actionsNativesLegacy[] =
{
	{ "ActionsManager.Allocate", NAT_action_Allocate },
	{ "ActionsManager.Deallocate", NAT_action_Deallocate },
	{ "ActionsManager.Iterator", NAT_action_GetEntityActions },
	{ "ActionsManager.GetAction", NAT_action_GetEntityAction },
	{ "ActionsManager.Create", NAT_action_Create },

	{ NULL, NULL }
};