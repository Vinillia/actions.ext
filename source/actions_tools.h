#pragma once

#ifndef _INCLUDE_ACTIONS_TOOLS_H
#define _INCLUDE_ACTIONS_TOOLS_H

#include <vector>
#include <functional>

namespace SourceMod
{
	class IGameConfig;
}

class IIntention;
class INextBot;
class CBaseEntity;
class INextBotEventResponder;

namespace vtable
{
	inline void* get(void* instance, int offset)
	{
		void** vtable = *reinterpret_cast<void***>(instance);
		return vtable[offset];
	}

	template<typename K, typename... T>
	inline K call(void* instance, int offset, T... args)
	{
		using fn_t = K(__thiscall*)(void*, T...);
		fn_t fn = reinterpret_cast<fn_t>(get(instance, offset));
		return fn(instance, args...);
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

public:
	bool GetEntityActions(CBaseEntity* entity, std::vector<nb_action_ptr>& vec);

	inline CBaseEntity* GetEntity(INextBot* bot) const;
	inline INextBot* MyNextBotPointer(CBaseEntity* entity) const;

protected:
	std::function<void(nb_action_ptr, std::vector<nb_action_ptr>&)> m_insertAction;
	int m_Entity_MyNextBotPointer_Offset;
	int m_NextBot_GetEntity_Offset;
};

inline CBaseEntity* ActionsTools::GetEntity(INextBot* bot) const
{
	return vtable::call<CBaseEntity*>(bot, m_NextBot_GetEntity_Offset);
}

inline INextBot* ActionsTools::MyNextBotPointer(CBaseEntity* entity) const
{
	return vtable::call<INextBot*>(entity, m_Entity_MyNextBotPointer_Offset);
}

extern ActionsTools* g_pActionsTools;

#endif // !_INCLUDE_ACTIONS_TOOLS_H
