#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"

class CExtBehaviorActions : public SDKExtension, public IPluginsListener
{
public:
	virtual void OnPluginUnloaded(IPlugin* plugin);

	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	virtual void SDK_OnUnload();
	virtual void SDK_OnAllLoaded();
	// virtual void SDK_OnPauseChange(bool paused);
	// virtual bool QueryRunning(char *error, size_t maxlen);
public:
#if defined SMEXT_CONF_METAMOD

	// virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	// virtual bool SDK_OnMetamodUnload(char *error, size_t maxlen);
	// virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlen);
#endif
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_