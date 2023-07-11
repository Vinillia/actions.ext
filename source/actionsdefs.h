#pragma once

#ifndef _ICNLUDE_ACTIONSDEFS_H
#define _ICNLUDE_ACTIONSDEFS_H

class CBaseEntity;

template<class Actor>
class Behavior;

template<class Actor>
class Action;

using nb_behavior = Behavior<CBaseEntity>;
using nb_behavior_ptr = nb_behavior*;

using nb_action = Action<CBaseEntity>;
using nb_action_ptr = nb_action*;

#define MsgSM(fmt, ...) g_pSM->LogMessage(myself, fmt, ##__VA_ARGS__)
#define WarningSM(fmt, ...) g_pSM->LogError(myself, fmt, ##__VA_ARGS__)

#ifdef _WIN32
	void* operator new(size_t size);
	void  operator delete(void* const block);
#endif

#endif // !_ICNLUDE_ACTIONSDEFS_H
