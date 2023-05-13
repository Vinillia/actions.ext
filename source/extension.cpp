#include "extension.h"
#include "hook.h"

#include "actions_processor.h"
#include "actions_pubvars.h"
#include "actions_propagation.h"
#include "actions_manager.h"
#include "actions_pubvars.h"
#include "actions_commands.h"
#include "actions_component.h"

#include "actions_natives.h"
#include "actions_legacy.h"
#include "actions_legacy_listeners.h"
#include <CDetour/detours.h>

#include <compat_wrappers.h>

SDKActions g_sdkActions;
SMEXT_LINK(&g_sdkActions);

CGlobalVars* gpGlobals = nullptr; 
ICvar* icvar = nullptr;
// IBinTools* bintools = nullptr;

extern void InitVirtualMap();
extern class NextBotManager& TheNextBots(void* pfn = nullptr);

ConVar ext_actions_debug("ext_actions_debug", "0", 0, "1 - Enable debug, 0 - Disable debug");

bool SDKActions::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	m_isNextBotDebugSupported = false;

	if (!gameconfs->LoadGameConfigFile("actions.games", &m_pConfig, error, maxlen))
		return false;

	void* pTheNextBots = nullptr;
	if (m_pConfig->GetMemSig("TheNextBots", &pTheNextBots))
	{
		m_isNextBotDebugSupported = true;
		TheNextBots(pTheNextBots);
	}

	InitVirtualMap();
	g_publicsManager.InitializePublicVariables();

	CDetourManager::Init(g_pSM->GetScriptingEngine(), m_pConfig);

	sharesys->AddNatives(myself, g_actionsNatives);
	sharesys->AddNatives(myself, g_actionsNativesLegacy);
	RegisterLegacyNatives();

	plsys->AddPluginsListener(this);
	sharesys->RegisterLibrary(myself, "actionslib");
	return true;
}

void SDKActions::SDK_OnAllLoaded()
{
	NextBotDebugHistory = icvar->FindVar("nb_debug_history");

	m_fwdOnActionCreated = forwards->CreateForward("OnActionCreated", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	m_fwdOnActionDestroyed = forwards->CreateForward("OnActionDestroyed", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);

	CreateActionsHook();
	//SM_GET_LATE_IFACE(BINTOOLS, bintools);
}

bool SDKActions::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late) 
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	g_pCVar = icvar;
	CONVAR_REGISTER(this);
	gpGlobals = ismm->GetCGlobals();
	return true;
};

void SDKActions::SDK_OnUnload()
{
	forwards->ReleaseForward(m_fwdOnActionCreated);
	forwards->ReleaseForward(m_fwdOnActionDestroyed);
	plsys->RemovePluginsListener(this);
	StopActionProcessing();
	
	ActionComponent::UnRegisterComponents();
	
	if (m_pConfig)
		gameconfs->CloseGameConfigFile(m_pConfig);
	
	DestroyActionsHook();
}

bool SDKActions::QueryRunning(char* error, size_t maxlength)
{
	// SM_CHECK_IFACE(BINTOOLS, bintools);
	return true;
}

void SDKActions::OnPluginLoaded(IPlugin* plugin)
{
	g_publicsManager.SyncPlugin(plugin->GetBaseContext());
}

void SDKActions::OnPluginUnloaded(IPlugin* plugin)
{
	ActionComponent::OnPluginUnloaded(plugin->GetBaseContext());
	g_actionsManager.ClearUserDataIdentity(plugin->GetBaseContext());
	g_actionsPropagationPre.RemoveListener(plugin->GetBaseContext());
	g_actionsPropagationPost.RemoveListener(plugin->GetBaseContext());
}

void SDKActions::OnActionCreated(nb_action_ptr action)
{
	m_fwdOnActionCreated->PushCell((cell_t)action);
	m_fwdOnActionCreated->PushCell(g_actionsManager.GetActionActorEntIndex(action));
	m_fwdOnActionCreated->PushString(action->GetName());
	m_fwdOnActionCreated->Execute();
}

void SDKActions::OnActionDestroyed(nb_action_ptr action)
{
	g_actionsPropagationPre.RemoveActionListeners(action);
	g_actionsPropagationPost.RemoveActionListeners(action);

	m_fwdOnActionDestroyed->PushCell((cell_t)action);
	m_fwdOnActionDestroyed->PushCell(g_actionsManager.GetActionActorEntIndex(action));
	m_fwdOnActionDestroyed->PushString(action->GetName());
	m_fwdOnActionDestroyed->Execute();
}

bool SDKActions::RegisterConCommandBase(ConCommandBase* command)
{
	return META_REGCVAR(command);
}

#ifdef _WIN32
void* _Use_decl_annotations_ operator new(size_t size)				{ return g_pMemAlloc->Alloc(size); }
void  _Use_decl_annotations_ operator delete(void* const block)		{ return g_pMemAlloc->Free(block); }
#endif