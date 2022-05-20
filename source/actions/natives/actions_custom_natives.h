
cell_t NAT_CreatePluginAction(IPluginContext* pContext, const cell_t* params)
{
	char* name;
	pContext->LocalToString(params[1], &name);
	
	PluginAction* action = new PluginAction(name);
	g_pActionsManager->AddPending(action);
		
	return (cell_t)action;
}

sp_nativeinfo_t g_ActionCustomNatives[] =
{
	{ "ActionsManager.Create", NAT_CreatePluginAction },
	{ NULL, NULL }
};