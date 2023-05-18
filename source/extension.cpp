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

ConVar ext_actions_debug("ext_actions_debug", "0", FCVAR_NONE, "1 - Enable debug, 0 - Disable debug");
ConVar ext_actions_debug_memory("ext_actions_debug_memory", "0", FCVAR_NONE, "Log component creation/deletion");

ConVar* developer = nullptr;
ConVar* NextBotDebugHistory = nullptr;

CGlobalVars* gpGlobals = nullptr; 
ICvar* icvar = nullptr;

IActionComponentDispatch g_componentDispatch;

extern void InitVirtualMap();
extern class NextBotManager& TheNextBots(void* pfn = nullptr);

bool SDKActions::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	m_isNextBotDebugSupported = false;
	m_htActionComponent = 0;

	if (!gameconfs->LoadGameConfigFile("actions.games", &m_pConfig, error, maxlen))
		return false;

	void* pTheNextBots = nullptr;
	if (m_pConfig->GetMemSig("TheNextBots", &pTheNextBots))
	{
		m_isNextBotDebugSupported = true;
		TheNextBots(pTheNextBots);
	}
	
	HandleError err;
	if (!CreateHandleTypes(&err))
	{
		V_snprintf(error, maxlen, "Failed to create handle type (error: %i)", err);
		return false;
	}

	InitVirtualMap();
	g_publicsManager.InitializePublicVariables();

	CDetourManager::Init(g_pSM->GetScriptingEngine(), m_pConfig);

	sharesys->AddNatives(myself, g_actionsNatives);
	sharesys->AddNatives(myself, g_actionsNativesLegacy);
	RegisterLegacyNatives();

	plsys->AddPluginsListener(this);
	playerhelpers->AddClientListener(this);

	sharesys->RegisterLibrary(myself, "actionslib");
	return true;
}

void SDKActions::SDK_OnAllLoaded()
{
	NextBotDebugHistory = icvar->FindVar("nb_debug_history");
	developer = icvar->FindVar("developer");

	m_fwdOnActionCreated = forwards->CreateForward("OnActionCreated", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);
	m_fwdOnActionDestroyed = forwards->CreateForward("OnActionDestroyed", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);

	CreateActionsHook();
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
	if (m_pConfig)
		gameconfs->CloseGameConfigFile(m_pConfig);

	if (m_htActionComponent)
		g_pHandleSys->RemoveType(m_htActionComponent, myself->GetIdentity());

	forwards->ReleaseForward(m_fwdOnActionCreated);
	forwards->ReleaseForward(m_fwdOnActionDestroyed);
	plsys->RemovePluginsListener(this);
	playerhelpers->RemoveClientListener(this);

	/* 
	* Handle sys can handle this 
	* ActionComponent::UnRegisterComponents();
	*/

	StopActionProcessing();
	DestroyActionsHook();
}

void SDKActions::OnPluginLoaded(IPlugin* plugin)
{
	g_publicsManager.SyncPlugin(plugin->GetBaseContext());
}

void SDKActions::OnPluginUnloaded(IPlugin* plugin)
{
	/*
	* Handle sys can handle this
	* ActionComponent::OnPluginUnloaded(plugin->GetBaseContext());
	*/

	g_actionsManager.ClearUserDataIdentity(plugin->GetBaseContext());
	g_actionsPropagationPre.RemoveListener(plugin->GetBaseContext());
	g_actionsPropagationPost.RemoveListener(plugin->GetBaseContext());
}

void SDKActions::OnActionCreated(nb_action_ptr action)
{
	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f:%i: NEW ACTION %s ( 0x%X )", gpGlobals->curtime, g_actionsManager.GetActionActorEntIndex(action), action->GetName(), action);

	m_fwdOnActionCreated->PushCell((cell_t)action);
	m_fwdOnActionCreated->PushCell(g_actionsManager.GetActionActorEntIndex(action));
	m_fwdOnActionCreated->PushString(action->GetName());
	m_fwdOnActionCreated->Execute();
}

void SDKActions::OnActionDestroyed(nb_action_ptr action)
{
	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f:%i: DELETE ACTION %s ( 0x%X )", gpGlobals->curtime, g_actionsManager.GetActionActorEntIndex(action), action->GetName(), action);

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

bool SDKActions::CreateHandleTypes(HandleError* err)
{
	//	tacc.access[HTypeAccess_Create] = true;
	//	tacc.access[HTypeAccess_Inherit] = true;

	m_htActionComponent = g_pHandleSys->CreateType("ActionComponent", &g_componentDispatch, 0, NULL, NULL, myself->GetIdentity(), err);
	return m_htActionComponent != 0;
}

void SDKActions::OnClientDisconnecting(int client)
{
	IGamePlayer* player = playerhelpers->GetGamePlayer(client);

	if (!player->IsFakeClient())
		return;

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(client);

	if (entity == nullptr)
		return;

	ActionComponent::DestroyComponents(entity);
}

void IActionComponentDispatch::OnHandleDestroy(HandleType_t type, void* object)
{
	ActionComponent* component = (ActionComponent*)object;
	component->OnHandleDestroy(type);
}

bool IActionComponentDispatch::GetHandleApproxSize(HandleType_t type, void* object, unsigned int* pSize)
{
	ActionComponent* component = (ActionComponent*)object;
	return component->GetHandleApproxSize(type, pSize);
}

#ifdef _WIN32
void* _Use_decl_annotations_ operator new(size_t size)				{ return g_pMemAlloc->Alloc(size); }
void  _Use_decl_annotations_ operator delete(void* const block)		{ return g_pMemAlloc->Free(block); }
#endif