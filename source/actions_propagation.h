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
struct hexecution
{
	ResultType rt;
	T rv;
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

	template<typename RETURN, typename ...Args>
	ResultType ProcessMethod(nb_action_ptr action, HashValue hash, RETURN* result, Args&&... args)
	{
		auto& list = m_actionsListeners[action][hash];

		ResultType returnResult, executeResult = Pl_Continue;
		returnResult = executeResult;
		m_isInExecution = true;

		for (auto iter = list.begin(); iter != list.end(); iter++)
		{
			IPluginFunction* fn = iter->fn;

			ProcessMethodArg<nb_action_ptr>(fn, std::forward<nb_action_ptr>(action));
			(ProcessMethodArg<Args>(fn, std::forward<Args>(args)), ...);

			if constexpr (!std::is_null_pointer_v<RETURN>)
			{
				if constexpr (is_action_result_v<RETURN>)
				{
					fn->PushCell((cell_t)result);
				}
				else
				{
					fn->PushCellByRef((cell_t*)result);
				}
			}

			fn->Execute((cell_t*)&executeResult);

			if (executeResult > returnResult)
				returnResult = executeResult;
		}
		
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
