
cell_t NAT_CreatePluginAction(IPluginContext* pContext, const cell_t* params)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	return (cell_t)(new PluginAction(name));
}

sp_nativeinfo_t g_ActionCustomNatives[] =
{
	{ "ActionsManager.Create", NAT_CreatePluginAction },
	{ NULL, NULL }
};