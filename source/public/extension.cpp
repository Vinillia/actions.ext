#include "extension.h"

#include "actions_manager.h"
#include "actions_processor.h"
#include "actions_cquery.h"
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

CGlobalVars *gpGlobals;
ICvar *icvar;

ConVar ext_actions_debug("ext_actions_debug", "0", 0, "1 - Enable debug, 0 - Disable debug");

bool CExtBehaviorActions::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	if (!ConfigureHooks())
	{
		snprintf(error, maxlen, "Failed to configure hooks");
		return false;
	}

	if (!ConfigureContextualHooks())
	{
		snprintf(error, maxlen, "Failed to configure contextual hooks");
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
	CreateHooks();
}

void CExtBehaviorActions::SDK_OnUnload()
{
	GetOffsetsManager()->ReleaseConfig();
	plsys->RemovePluginsListener(this);
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