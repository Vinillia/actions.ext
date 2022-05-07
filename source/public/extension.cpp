#include "extension.h"

#include "actions_manager.h"
#include "actions_processor.h"
#include "actions_custom.h"
#include "actions_commands.h"

#include "actions_natives.h"
#include "actions_processors_natives.h"
#include "actions_custom_natives.h"

#include "hooks.h"
#include <compat_wrappers.h>

#pragma comment(lib, "legacy_stdio_definitions.lib")

CExtBehaviorActions g_BehaviorActions;
SMEXT_LINK(&g_BehaviorActions);

IGameConfig* g_pGameConf;
CGlobalVars *gpGlobals;
ICvar *icvar;

bool CExtBehaviorActions::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	char szError[255];

	if (!gameconfs->LoadGameConfigFile("l4d_actions", &g_pGameConf, szError, sizeof(szError)))
	{
		snprintf(error, maxlen, "Failed to load game config file: %s", szError);
		return false;
	}

	sharesys->RegisterLibrary(myself, "actionslib");
	plsys->AddPluginsListener(this);

	sharesys->AddNatives(myself, g_ActionNatives);
	sharesys->AddNatives(myself, g_ActionProcessorNatives);
	sharesys->AddNatives(myself, g_ActionCustomNatives);
	return true;
}

void CExtBehaviorActions::SDK_OnAllLoaded()
{
	ReconfigureHooks();
	CreateHooks(g_pGameConf);
}

void CExtBehaviorActions::SDK_OnUnload()
{
	plsys->RemovePluginsListener(this);
	gameconfs->CloseGameConfigFile(g_pGameConf);
}

void CExtBehaviorActions::OnPluginUnloaded(IPlugin* plugin)
{
	g_pActionsPropagatePre->RemoveListeners(plugin->GetBaseContext());
	g_pActionsPropagatePost->RemoveListeners(plugin->GetBaseContext());
}

bool CExtBehaviorActions::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	gpGlobals = ismm->GetCGlobals();
	CONVAR_REGISTER(this);
	return true;
}

bool CExtBehaviorActions::RegisterConCommandBase(ConCommandBase* command)
{
	return META_REGCVAR(command);
}

#ifndef __linux__
	void* operator new(size_t size)				{ return g_pMemAlloc->Alloc(size); }
	void  operator delete(void* const block)	{ return g_pMemAlloc->Free(block); }
#endif