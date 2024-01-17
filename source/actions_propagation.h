#pragma once

#ifndef _ICNLUDE_ACTIONS_PROPAGATION_H
#define _ICNLUDE_ACTIONS_PROPAGATION_H

#include "smsdk_ext.h"
#include "actionsdefs.h"

#include "actions_pubvars.h"
#include "actions_manager.h"

#include <unordered_map>
#include <vector>
#include <type_traits>
#include <list>

template<class Actor>
struct ActionResult;

template<class Actor>
struct EventDesiredResult;

template<typename T>
struct is_action_result : public std::is_same<T, ActionResult<CBaseEntity>> {};

template<typename T>
struct is_action_desire_result : public std::is_same<T, EventDesiredResult<CBaseEntity>> {};

template<typename T>
inline constexpr bool is_action_result_v = is_action_result<T>::value || is_action_desire_result<T>::value;

template<typename T>
struct handler_result
{
	ResultType plresult;
	T value;
};

struct ActionListener
{
	ActionListener() = default;
	ActionListener(HashValue _hash, IPluginFunction* _fn) : hash(_hash), fn(_fn)
	{
	}

	HashValue hash;
	IPluginFunction* fn;
};

using MethodListeners = std::unordered_map<HashValue, std::list<ActionListener>>;
using ActionListeners = std::unordered_map<nb_action_ptr, MethodListeners>;

extern ConVar ext_actions_debug_memory;

template<typename T>
static inline bool is_result_same(const T& left, const T& right)
{
	return left.m_action == right.m_action && left.m_type == right.m_type;
}

template<>
static inline bool is_result_same(const EventDesiredResult<CBaseEntity>& left, const EventDesiredResult<CBaseEntity>& right)
{
	return left.m_priority == right.m_priority && is_result_same((const ActionResult<CBaseEntity>&)left, (const ActionResult<CBaseEntity>&)right);
}

class ActionPropagation
{
	struct runtime_deletor
	{
		using container_t = MethodListeners::mapped_type;
		using item_t = container_t::iterator;

		runtime_deletor(container_t* data, const item_t& item)
			: data(data), item(item)
		{
		}

		container_t* data;
		item_t item;
	};

public:
	ActionPropagation();
	~ActionPropagation();

public:
	template<typename T>
	inline void ProcessMethodArg(IPluginFunction* fn, T&& arg)
	{
		using type = std::decay_t<T>;

		if constexpr (std::is_same_v<type, float>)
		{
			fn->PushFloat((float)arg);
		}
		else if constexpr (std::is_same_v<type, char*> || std::is_same_v<type, const char*>)
		{
			fn->PushString((char*)arg);
		}
		else if constexpr (std::is_same_v<type, Vector>)
		{
			fn->PushArray((cell_t*)&arg, sizeof(Vector));
		}
		else if constexpr (std::is_same_v<type, CBaseEntity*> || std::is_same_v<type, CBaseCombatCharacter*>)
		{
			cell_t entity = -1;

			if (arg != nullptr)
				entity = gamehelpers->EntityToBCompatRef((CBaseEntity*)arg);

			fn->PushCell(entity);
		}
		else if constexpr (std::is_same_v<type, int> || std::is_pointer_v<T> || std::is_enum_v<T>)
		{
			fn->PushCell((cell_t)arg);
		}
		else if constexpr (std::is_same_v<type, CTakeDamageInfo>)
		{
			fn->PushCell((cell_t)&arg);
		}
		else
		{
			fn->PushCell((cell_t)arg);
		}
	}

	template<typename TReturn, typename ...Args>
	ResultType ProcessMethod(nb_action_ptr action, HashValue hash, TReturn* result, Args&&... args)
	{
		auto& listeners = m_actionsListeners[action][hash];

		ResultType returnResult, executeResult = Pl_Continue;

		returnResult = executeResult;
		m_isInExecution = true;
		g_actionsManager.SetRuntimeAction(action);

		for (auto it = listeners.begin(); it != listeners.end(); it++)
		{
			IPluginFunction* fn = it->fn;
			TReturn priorResult = *result;

			ProcessMethodArg<nb_action_ptr>(fn, std::forward<nb_action_ptr>(action));
			(ProcessMethodArg<Args>(fn, std::forward<Args>(args)), ...);

			if constexpr (!std::is_null_pointer_v<TReturn>)
			{
				if constexpr (is_action_result_v<TReturn>)
				{
					fn->PushCell((cell_t)result);
				}
				else
				{
					fn->PushCellByRef((cell_t*)result);
				}
			}

			fn->Execute((cell_t*)&executeResult);

			if constexpr (is_action_result_v<TReturn>)
			{
				if (executeResult < Pl_Changed && !is_result_same(priorResult, *result))
				{
					// plugin changed result but returned Plugin_Continue. It's ok but we need to do something otherwise we might crash later.
					fn->GetParentRuntime()->GetDefaultContext()->BlamePluginError(fn, "Changing result with Plugin_Continue is an error");

					// delete plugin's action and use prior as the most safest result
					if (result->m_action && result->m_action != priorResult.m_action)
					{
						delete result->m_action;
						*result = priorResult;
					}

					RemoveListener(action, it->hash, it->fn);
				}
				
				if (priorResult.m_action && priorResult.m_action != result->m_action)
				{
					if (ext_actions_debug_memory.GetBool())
						MsgSM("%.3f:%i: DELETE ACTION %s ( 0x%X )", gpGlobals->curtime, g_actionsManager.GetActionActorEntIndex(action), priorResult.m_action->GetName(), priorResult.m_action);

					// delete an outdated action
					delete priorResult.m_action;
					priorResult.m_action = nullptr;
				}
			}

			if (executeResult > returnResult)
			{
				// new result is more superior
				returnResult = executeResult;
			}
		}

		g_actionsManager.SetRuntimeAction(nullptr);

		m_isInExecution = false;
		ProcessDeletors();

		return returnResult;
	}

public:
	bool AddListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn);
	bool RemoveListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn);
	bool RemoveListener(nb_action_ptr action, HashValue hash, IPluginContext* ctx);
	void RemoveListener(IPluginContext* ctx);
	void RemoveActionListeners(nb_action_ptr action);
	ActionListener* HasListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn);

private:
	inline bool HandleRemoveProcess(runtime_deletor::container_t* container, runtime_deletor::item_t& item);
	inline void ProcessDeletors();

protected:
	ActionListeners m_actionsListeners;

private:
	std::vector<runtime_deletor> m_deletors;
	bool m_isInExecution;
};

inline bool ActionPropagation::HandleRemoveProcess(runtime_deletor::container_t* container, runtime_deletor::item_t& item)
{
	if (!m_isInExecution)
		return false;

	m_deletors.emplace_back(container, item);
	return true;
}

inline void ActionPropagation::ProcessDeletors()
{
	for (auto& iter : m_deletors)
	{
		iter.data->erase(iter.item);
	}

	m_deletors.clear();
}

extern ActionPropagation g_actionsPropagationPre;
extern ActionPropagation g_actionsPropagationPost;

#endif // !_ICNLUDE_ACTIONS_PROPAGATION_H
