#ifndef _INCLUDE_ACTIONS_HELPERS_H
#define _INCLUDE_ACTIONS_HELPERS_H

#include <utility>
#include <map>
#include <string>

#include "utils.h"
#include "extension.h"
#include "offset_manager.h"

#include "actions_manager.h"
#include "actions_propagate.h"

#include "NextBotBehavior.h"
#include "NextBotIntentionInterface.h"

#define DEFINE_PROCESSOR(unique, name, ret, ...)	\
	HandlerProcessor<unique, ret, ##__VA_ARGS__> name

#define RECONFIGURE_MANUALHOOK(hookname) \
		do { \
			int32_t vtableidx = GetOffsetsManager()->RequestOffset(#hookname); \
			SH_MANUALHOOK_RECONFIGURE(hookname, vtableidx, 0, 0); \
		} while(0)

#define START_PROCESSOR(hookname, varname)	\
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(m_action), SH_STATIC(decltype(varname)::Process), false); \
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(m_action), SH_STATIC(decltype(varname)::ProcessPost), true); \
	varname.vtableindex = g_CachedOffsets->at(#hookname); \
	varname.name = #hookname

extern const std::map<std::string, int32_t>* g_CachedOffsets; 


extern void ExecuteProcessor(CBaseEntity* entity, Action<void>* action);

extern const bool ConfigureHooks();
extern void HookIntentions(IGameConfig* config);

extern OffsetManager* GetOffsetsManager();
extern const std::map<std::string, int32_t>* g_CachedOffsets;

#endif // _INCLUDE_ACTIONS_HELPERS_H