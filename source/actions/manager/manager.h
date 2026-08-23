#pragma once

#ifndef _INCLUDE_ACTIONS_MANAGER_H
#define _INCLUDE_ACTIONS_MANAGER_H

#include "smsdk_ext.h"

#include "actions/defs.h"
#include "actions/runtime/pubvars.h"

#include <amtl/am-hashset.h>
#include <amtl/am-hashmap.h>

#include <unordered_map>
#include <unordered_set>
#include <any>
#include <vector>
#include <stack>
#include <stdexcept>
#include <variant>

#include <NextBotBehavior.h>

template<typename T>
struct ActionResult;

template<typename T>
struct EventDesiredResult;

using ActionUserData = std::variant<
    cell_t,
    Vector,
    std::string
>;

class ActionsManager
{
private:
	using ActionsContanier = ke::HashSet<nb_action_ptr, ke::PointerPolicy<nb_action>>;
	using UserDataMap = std::unordered_map<std::string_view, ActionUserData>;

public:
	using ActionId = int32_t;
	static inline constexpr ActionId null_action_id = 0;

private:
	// there are extremly rare cases when the same multiple actions can be registered for the same entity besides user's intervention
	using entity_action_map = std::unordered_multimap<ActionId, nb_action_ptr>;

	static inline ActionId global_action_id = 1;

	enum class RuntimeResultKind : uint8_t
	{
		Unknown = 0,
		ActionResult,
		EventDesiredResult,
	};

	struct RuntimeResultEntry
	{
		void* ptr = nullptr;
		RuntimeResultKind kind = RuntimeResultKind::Unknown;

		static RuntimeResultEntry FromRaw(void* p, RuntimeResultKind k = RuntimeResultKind::Unknown) noexcept
		{
			RuntimeResultEntry e;
			e.ptr = p;
			e.kind = k;
			return e;
		}

		static RuntimeResultEntry FromAction(ActionResult<CBaseEntity>* p) noexcept
		{
			return FromRaw(p, RuntimeResultKind::ActionResult);
		}

		static RuntimeResultEntry FromDesired(EventDesiredResult<CBaseEntity>* p) noexcept
		{
			return FromRaw(p, RuntimeResultKind::EventDesiredResult);
		}
	};

public:
	ActionsManager();
	~ActionsManager();

	inline void Add(nb_action_ptr const action, CBaseEntity* const entity);

	void Add(nb_action_ptr const action);
	void Remove(nb_action_ptr const action);

	inline void AddPending(nb_action_ptr const action);
	inline void RemovePending(nb_action_ptr const action);

	void ClearUserData(nb_action_ptr action);

	void ProcessResult(nb_action_ptr action, const ActionResult<CBaseEntity>& result);
	void ProcessInitialContainedAction(const ResultType& pl, nb_action_ptr parent, nb_action_ptr oldaction, nb_action_ptr newaction);

	inline void PushRuntimeResult(ActionResult<CBaseEntity>* result);
	inline void PushRuntimeResult(EventDesiredResult<CBaseEntity>* result);
	inline void PushRuntimeResult(void* ptr);

	inline void PopRuntimeResult() noexcept;

	inline RuntimeResultEntry* TopRuntimeResult();

	inline void SetRuntimeAction(nb_action_ptr action);
	inline nb_action_ptr GetRuntimeAction() const noexcept;

	ActionId RegisterActionID(std::string_view name);
	ActionId FindActionID(std::string_view name) const;

	nb_action_ptr LookupEntityAction(CBaseEntity* entity, ActionId id) const;
	nb_action_ptr LookupEntityAction(CBaseEntity* entity, const char* name) const;

	inline ActionResult<CBaseEntity>* GetActionRuntimeResult();
	inline EventDesiredResult<CBaseEntity>* GetActionRuntimeDesiredResult();

	inline void SetActionActor(nb_action_ptr action, CBaseEntity* actor) noexcept;
	inline CBaseEntity* GetActionActor(nb_action_ptr action) const noexcept;

	cell_t GetActionActorEntIndex(nb_action_ptr action);

	inline bool IsValidAction(nb_action_ptr action);

	template<typename T>
	void SetUserData(nb_action_ptr action, std::string_view key, T&& value)
	{
		m_actionsUserData[action][std::string(key)] =
			std::forward<T>(value);
	}

	template<typename T>
	bool GetUserData(
		nb_action_ptr action,
		std::string_view key,
		T& value
	) const
	{
		auto action_it = m_actionsUserData.find(action);
		if (action_it == m_actionsUserData.end())
			return false;

		auto data_it = action_it->second.find(std::string(key));
		if (data_it == action_it->second.end())
			return false;

		if (const auto* ptr = std::get_if<T>(&data_it->second))
		{
			value = *ptr;
			return true;
		}

		return false;
	}

protected:
	inline bool IsPending(nb_action_ptr action);

	virtual void OnActionCreated(nb_action_ptr action);
	virtual void OnActionDestroyed(nb_action_ptr action);

	void StoreEntityAction(nb_action_ptr action, ActionId id);
	void DeleteEntityAction(nb_action_ptr action, ActionId id);

private:
	ActionsContanier m_actions;
	std::unordered_set<nb_action_ptr> m_actionsPending;
	std::unordered_map<nb_action_ptr, CBaseEntity*> m_actionActor;

	std::unordered_map<nb_action_ptr, UserDataMap> m_actionsUserData;

	std::unordered_map<std::string, ActionId> m_actionIDs;

	std::vector<entity_action_map> m_entityActions;
	
	nb_action_ptr m_pRuntimeAction = nullptr;

	std::stack<RuntimeResultEntry> m_runtimeResult;
};

inline void ActionsManager::Add(nb_action_ptr const action, CBaseEntity* const entity)
{
	SetActionActor(action, entity);
	Add(action);
}

inline bool ActionsManager::IsValidAction(nb_action_ptr action)
{
	if (!action)
		return false;

	if (IsPending(action))
		return true;

	return m_actions.find(action).found();
}

inline bool ActionsManager::IsPending(nb_action_ptr action)
{
	return m_actionsPending.find(action) != m_actionsPending.cend();
}

inline void ActionsManager::AddPending(nb_action_ptr const action)
{
	m_actionsPending.insert(action);
}

inline void ActionsManager::RemovePending(nb_action_ptr const action)
{
	m_actionsPending.erase(action);
}

inline void ActionsManager::PushRuntimeResult(ActionResult<CBaseEntity>* result)
{
	m_runtimeResult.push(RuntimeResultEntry::FromAction(result));
}

inline void ActionsManager::PushRuntimeResult(EventDesiredResult<CBaseEntity>* result)
{
	m_runtimeResult.push(RuntimeResultEntry::FromDesired(result));
}

inline void ActionsManager::PushRuntimeResult(void* ptr)
{
	m_runtimeResult.push(RuntimeResultEntry::FromRaw(ptr, RuntimeResultKind::Unknown));
}

inline void ActionsManager::PopRuntimeResult() noexcept
{
	if (!m_runtimeResult.empty())
		m_runtimeResult.pop();
}

inline void ActionsManager::SetRuntimeAction(nb_action_ptr action)
{
	m_pRuntimeAction = action;
}

inline nb_action_ptr ActionsManager::GetRuntimeAction() const noexcept
{
	return m_pRuntimeAction;
}

inline ActionsManager::RuntimeResultEntry* ActionsManager::TopRuntimeResult()
{
	if (m_runtimeResult.empty())
		return nullptr;

	return &m_runtimeResult.top();
}

inline ActionResult<CBaseEntity>* ActionsManager::GetActionRuntimeResult()
{
	RuntimeResultEntry* entry = TopRuntimeResult();
	if (!entry)
		return nullptr;

	if (entry->kind == RuntimeResultKind::ActionResult || entry->kind == RuntimeResultKind::EventDesiredResult)
		return static_cast<ActionResult<CBaseEntity>*>(entry->ptr);

	return nullptr;
}

inline EventDesiredResult<CBaseEntity>* ActionsManager::GetActionRuntimeDesiredResult()
{
	RuntimeResultEntry* entry = TopRuntimeResult();
	if (!entry)
		return nullptr;

	if (entry->kind == RuntimeResultKind::EventDesiredResult)
		return static_cast<EventDesiredResult<CBaseEntity>*>(entry->ptr);

	return nullptr;
}

inline void ActionsManager::SetActionActor(nb_action_ptr action, CBaseEntity* actor) noexcept
{
	m_actionActor[action] = actor;
}

inline CBaseEntity* ActionsManager::GetActionActor(nb_action_ptr action) const noexcept
{
	auto r = m_actionActor.find(action);

	if (r == m_actionActor.cend())
	{
		return action->GetActor();
	}

	return r->second;
}

extern ActionsManager g_actionsManager;

#endif // !_INCLUDE_ACTIONS_MANAGER_H
