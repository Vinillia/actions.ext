#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"

class CExtBehaviorActions : public SDKExtension, public IPluginsListener, public IConCommandBaseAccessor
{
public:
	virtual void OnPluginUnloaded(IPlugin* plugin) override;

	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late) override;
	virtual void SDK_OnUnload() override;
	virtual void SDK_OnAllLoaded() override;

	virtual bool RegisterConCommandBase(ConCommandBase* command) override;

	// virtual void SDK_OnPauseChange(bool paused);
	// virtual bool QueryRunning(char *error, size_t maxlen);
public:
#if defined SMEXT_CONF_METAMOD
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late) override;
	// virtual bool SDK_OnMetamodUnload(char *error, size_t maxlen);
	// virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlen);
#endif
};

extern IGameConfig* g_pGameConf;
extern CGlobalVars *gpGlobals;
extern ICvar *icvar;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_