#pragma once

#ifndef _INCLUDE_ACTIONS_TOOLS_H
#define _INCLUDE_ACTIONS_TOOLS_H

#ifdef clamp
#undef clamp // Valve
#endif

#include <vector>
#include <functional>

#include "NextBotDebug.h"

namespace SourceMod
{
	class IGameConfig;
}

class IIntention;
class INextBotComponent;
class INextBot;
class CBaseEntity;
class INextBotEventResponder;
class Color;

namespace ine
{
	template<typename K, typename... T>
	inline K call_cdecl(void* addr, T... args)
	{
		using fn_t = K(__cdecl*)(T...);
		fn_t fn = reinterpret_cast<fn_t>(addr);
		return fn(args...);
	}

	template<typename K, typename... T>
	inline K call_std(void* addr, T... args)
	{
#ifdef WIN32
		using fn_t = K(__stdcall*)(T...);
#else
		using fn_t = K(__cdecl*)(T...);
#endif
		fn_t fn = reinterpret_cast<fn_t>(addr);
		return fn(args...);
	}

	template<typename K, typename... T>
	inline K call_this(void* addr, T... args)
	{
#ifdef WIN32
#if PLATFORM_64BITS
		using fn_t = K(__fastcall*)(T...);
#else
		using fn_t = K(__thiscall*)(T...);
#endif
		fn_t fn = reinterpret_cast<fn_t>(addr);
		return fn(args...);
#else
		return call_cdecl<K, T...>(addr, args...);
#endif
	}

	inline void* get_vtable_fn(void* instance, int offset)
	{
		void** vtable = *reinterpret_cast<void***>(instance);
		return vtable[offset];
	}

	template<typename K, typename... T>
	inline K call_vtable(void* instance, int offset, T&&... args)
	{
		return call_this<K, T...>(get_vtable_fn(instance, offset), instance, std::forward<T>(args)...);
	}
}

class ActionsTools
{
public:
	ActionsTools();
	virtual ~ActionsTools() {};

public:
	virtual bool OnIntentionReset(INextBot* bot, IIntention* intention);
	virtual bool LoadGameConfigFile(SourceMod::IGameConfig* config, char* error, size_t maxlen);
	virtual void GetIntentionActions(IIntention* intention, std::vector<nb_action_ptr>& vec);

	nb_action_ptr ActionContainedResponder(IIntention* intention);
	
protected:
	virtual IIntention* TryCastToIntentionComponent(INextBotComponent* component);

public:
	bool GetEntityActions(CBaseEntity* entity, std::vector<nb_action_ptr>& vec);

	inline CBaseEntity* GetEntity(INextBot* bot) const;
	inline INextBot* MyNextBotPointer(CBaseEntity* entity) const;

	template<typename ...Args>
	inline void DebugConColorMsg(INextBot* bot, NextBotDebugType debugType, const Color& color, const char* fmt, Args&&...);

	template<typename ...Args>
	inline void DebugConColorMsg(CBaseEntity* entity, NextBotDebugType debugType, const Color& color, const char* fmt, Args&&...);

	inline bool IsDebugging(INextBot* bot, NextBotDebugType type) const;
	inline bool IsDebugging(CBaseEntity* entity, NextBotDebugType type) const;
	inline bool IsDebugSupported() const;

protected:
	std::function<void(nb_action_ptr, std::vector<nb_action_ptr>&)> m_insertAction;
	int m_Entity_MyNextBotPointer_Offset;
	int m_NextBot_GetEntity_Offset;
	void* m_NextBot_DebugConColorMsg_Address;
	void* m_NextBot_IsDebugging_Address;
};

template<typename ...Args>
inline void ActionsTools::DebugConColorMsg(INextBot* bot, NextBotDebugType debugType, const Color& color, const char* fmt, Args&&... args)
{
	// seems illegal, dunno
	ine::call_std<void, INextBot*, NextBotDebugType, const Color&, const char*, Args...>(m_NextBot_DebugConColorMsg_Address, bot, debugType, color, fmt, args...);
}

template<typename ...Args>
inline void ActionsTools::DebugConColorMsg(CBaseEntity* entity, NextBotDebugType debugType, const Color& color, const char* fmt, Args&&... args)
{
	DebugConColorMsg<Args...>(MyNextBotPointer(entity), debugType, color, fmt, args...);
}

inline CBaseEntity* ActionsTools::GetEntity(INextBot* bot) const
{
	return ine::call_vtable<CBaseEntity*>(bot, m_NextBot_GetEntity_Offset);
}

inline INextBot* ActionsTools::MyNextBotPointer(CBaseEntity* entity) const
{
	return ine::call_vtable<INextBot*>(entity, m_Entity_MyNextBotPointer_Offset);
}

inline bool ActionsTools::IsDebugging(INextBot* bot, NextBotDebugType type) const
{
	return ine::call_this<bool, INextBot*, NextBotDebugType>(m_NextBot_IsDebugging_Address, bot, type);
}

inline bool ActionsTools::IsDebugging(CBaseEntity* entity, NextBotDebugType type) const
{
	return IsDebugging(MyNextBotPointer(entity), type);
}

inline bool ActionsTools::IsDebugSupported() const
{
	return m_NextBot_DebugConColorMsg_Address != nullptr &&
		m_NextBot_IsDebugging_Address != nullptr;
}

extern ActionsTools* g_pActionsTools;

#endif // !_INCLUDE_ACTIONS_TOOLS_H
