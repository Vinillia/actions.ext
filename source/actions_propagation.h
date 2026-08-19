#pragma once

#ifndef _ICNLUDE_ACTIONS_PROPAGATION_H
#define _ICNLUDE_ACTIONS_PROPAGATION_H

#include "smsdk_ext.h"
#include "actionsdefs.h"

#include "actions_pubvars.h"
#include "actions_manager.h"

#include "extension.h"

#include <unordered_map>
#include <vector>
#include <type_traits>

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

struct ActionListener
{
	ActionListener() = default;
	ActionListener(HashValue hash, IPluginFunction* fn, IPluginContext* context)
		: hash(hash), fn(fn), context(context)
	{
	}

	HashValue hash = 0;
	IPluginFunction* fn = nullptr;
	IPluginContext* context = nullptr;
	bool removed = false;
};

using ListenerVector = std::vector<ActionListener>;
using MethodListeners = std::unordered_map<HashValue, ListenerVector>;
using ActionListeners = std::unordered_map<nb_action_ptr, MethodListeners>;

extern ConVar ext_actions_debug_memory;

template<typename T>
inline bool is_result_same(const T& left, const T& right)
{
	return left.m_action == right.m_action && left.m_type == right.m_type;
}

template<>
inline bool is_result_same(const EventDesiredResult<CBaseEntity>& left, const EventDesiredResult<CBaseEntity>& right)
{
	return left.m_priority == right.m_priority && is_result_same((const ActionResult<CBaseEntity>&)left, (const ActionResult<CBaseEntity>&)right);
}

class ActionPropagation
{
	class execution_guard
	{
	public:
		explicit execution_guard(ActionPropagation& owner) noexcept
			: m_owner(owner)
		{
			++m_owner.m_executionDepth;
		}

		~execution_guard() noexcept
		{
			--m_owner.m_executionDepth;
			m_owner.CleanupRemovedListeners();
		}

		execution_guard(const execution_guard&) = delete;
		execution_guard& operator=(const execution_guard&) = delete;

	private:
		ActionPropagation& m_owner;
	};

public:
	ActionPropagation() = default;
	~ActionPropagation() = default;

public:
	template<typename T>
	inline void ProcessMethodArg(IPluginFunction* fn, T&& arg)
	{
		using type = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<type, float>)
		{
			fn->PushFloat((float)arg);
		}
		else if constexpr (std::is_same_v<type, CBaseEntity*> ||
			std::is_same_v<type, CBaseCombatCharacter*> ||
			std::is_same_v<type, const CBaseCombatCharacter*> ||
			std::is_same_v<type, CBaseCombatWeapon*>)
		{
			cell_t entity = -1;

			/* https://github.com/Vinillia/actions.ext/issues/24 */
			if ((void*)arg != (void*)0xFFFFFFFF && arg != nullptr)
				entity = gamehelpers->EntityToBCompatRef((CBaseEntity*)arg);

			fn->PushCell(entity);
		}
		else if constexpr (std::is_same_v<type, const INextBot*>)
		{
#ifdef _CONVERT_NEXTBOT_TO_ENTITY
			cell_t entity = -1;

			if (arg != nullptr)
			{
				CBaseEntity* pEntity = g_pActionsTools->GetEntity(const_cast<INextBot*&>(arg));

				if (pEntity)
					entity = gamehelpers->EntityToBCompatRef(pEntity);
			}

			fn->PushCell(entity);
#else
			fn->PushCell(ToPseudoAddress(arg));
#endif // _CONVERT_NEXTBOT_TO_ENTITY
		}
		else if constexpr (std::is_same_v<type, CGameTrace*> ||
			std::is_same_v<type, nb_action_ptr> ||
			std::is_same_v<type, const Path*> ||
			std::is_same_v<type, animevent_t*> ||
			std::is_same_v<type, AI_Response*> ||
			std::is_same_v<type, void*> ||
			std::is_same_v<type, CNavArea*> ||
			std::is_same_v<type, CKnownEntity*> ||
			std::is_same_v<type, const CKnownEntity*>)
		{
			fn->PushCell(ToPseudoAddress(arg));
		}
		else if constexpr (std::is_same_v<type, MoveToFailureType> ||
			std::is_same_v<type, int>)
		{
			fn->PushCell((cell_t)arg);
		}
		else if constexpr (std::is_same_v<type, CTakeDamageInfo>)
		{
			fn->PushCell(ToPseudoAddress(&arg));
		}
		else if constexpr (std::is_same_v<type, KeyValues*>)
		{
			// There should be a way to pass this as soucemod's handle
			fn->PushCell(ToPseudoAddress(arg));
		}
		else if constexpr (std::is_same_v<type, Vector>)
		{
			fn->PushArray(const_cast<cell_t*>(reinterpret_cast<const cell_t*>(&arg)), sizeof(Vector));
		}
		else if constexpr (std::is_same_v<type, const char*>)
		{
			fn->PushString(arg);
		}
		else
		{
			static_assert(std::is_same_v<type, void>, "Unsupported type");
			static_assert(!std::is_same_v<type, void>, "Unsupported type");
		}
	}

	template<typename TReturn, typename ...Args>
	ResultType ProcessMethod(nb_action_ptr action, HashValue hash, TReturn* result, Args&&... args)
	{
		auto* listeners = FindListeners(action, hash);
		if (!listeners || listeners->empty())
			return Pl_Continue;


		std::size_t dispatchCount = listeners->size();
		execution_guard guard(*this);

		ResultType returnResult = Pl_Continue;
		g_actionsManager.SetRuntimeAction(action);

		for (std::size_t i = 0; i < dispatchCount; ++i)
		{
			if ((*listeners)[i].removed)
				continue;

			IPluginFunction* const fn = (*listeners)[i].fn;
			if (!fn)
			{
				MarkRemoved((*listeners)[i]);
				continue;
			}

			ResultType executeResult = Pl_Continue;
			TReturn saveResult;

			if constexpr (!std::is_null_pointer_v<TReturn>)
			{
				saveResult = *result;
			}

			ProcessMethodArg<nb_action_ptr>(fn, std::forward<nb_action_ptr>(action));
			(ProcessMethodArg<Args>(fn, std::forward<Args>(args)), ...);

			if constexpr (!std::is_null_pointer_v<TReturn>)
			{
				if constexpr (is_action_result_v<TReturn>)
				{
					fn->PushCell(ToPseudoAddress(result));
				}
				else if constexpr (std::is_same_v<TReturn, Vector>)
				{
					fn->PushArray((cell_t*)result, sizeof(Vector), SM_PARAM_COPYBACK);
				}
				else
				{
					fn->PushCellByRef((cell_t*)result);
				}
			}

			fn->Execute((cell_t*)&executeResult);

			if constexpr (is_action_result_v<TReturn>)
			{
				if (executeResult < Pl_Changed && !is_result_same(saveResult, *result))
				{
					fn->GetParentRuntime()->GetDefaultContext()->BlamePluginError(fn, "Changing result with Plugin_Continue is an error");

					if (result->m_action && result->m_action != saveResult.m_action)
					{
						delete result->m_action;
						*result = saveResult;
					}

					MarkRemoved((*listeners)[i]);
				}

				if (saveResult.m_action && saveResult.m_action != result->m_action)
				{
					if (ext_actions_debug_memory.GetBool())
						MsgSM("%.3f:%i: DELETE ACTION %s ( 0x%X )", gpGlobals->curtime, g_actionsManager.GetActionActorEntIndex(action), saveResult.m_action->GetName(), saveResult.m_action);

					delete saveResult.m_action;
					saveResult.m_action = nullptr;
				}
			}
			else if constexpr (!std::is_null_pointer_v<TReturn> && std::is_pointer_v<TReturn>)
			{
				if (saveResult != *result)
					*result = FromPseudoAddress<TReturn>(*reinterpret_cast<cell_t*>(result));
			}

			if (executeResult <= Pl_Continue)
			{
				if constexpr (!std::is_null_pointer_v<TReturn>)
				{
					*result = saveResult;
				}
			}
			else if (executeResult > returnResult)
			{
				returnResult = executeResult;
			}
		}

		g_actionsManager.SetRuntimeAction(nullptr);

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
	ListenerVector* FindListeners(nb_action_ptr action, HashValue hash) noexcept;
	bool MarkRemoved(ActionListener& listener) noexcept;
	void CleanupRemovedListeners() noexcept;

protected:
	ActionListeners m_actionsListeners;

private:
	std::size_t m_executionDepth = 0;
	bool m_cleanupPending = false;
};

extern ActionPropagation g_actionsPropagationPre;
extern ActionPropagation g_actionsPropagationPost;

#endif // !_ICNLUDE_ACTIONS_PROPAGATION_H
