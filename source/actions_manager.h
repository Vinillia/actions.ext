#pragma once

#ifndef _INCLUDE_ACTIONS_MANAGER_H
#define _INCLUDE_ACTIONS_MANAGER_H

#include "smsdk_ext.h"
#include "actionsdefs.h"
#include "actions_pubvars.h"

#include <amtl/am-hashset.h>
#include <amtl/am-hashmap.h>

#include <unordered_map>
#include <unordered_set>
#include <any>
#include <vector>
#include <stack>
#include <optional>

#include <NextBotBehavior.h>

template<typename T>
struct ActionResult;

template<typename T>
struct EventDesiredResult;

template<typename T, size_t N>
class ReservedVector
{
public:
	ReservedVector() : m_data(N) {}

	std::vector<T>& operator*() { return m_data; }
	const std::vector<T>& operator*() const { return m_data; }
	std::vector<T>* operator->() { return &m_data; }
	const std::vector<T>* operator->() const { return &m_data; }

private:
	std::vector<T> m_data;
};

struct ActionUserData
{
	struct Hash
	{
		std::size_t operator ()(const std::string_view& str) const noexcept
		{;
			return ke::FastHashCharSequence(str.data(), str.length());
		}
	};

	struct Equal
	{
		bool operator ()(const std::string_view& left, const std::string_view& right) const noexcept
		{
			return left.compare(right) == 0;
		}
	};
};

struct ActionUserDataIdentity
{
	struct Hash
	{
		std::size_t operator ()(const ActionUserDataIdentity& indentity) const noexcept
		{
			std::size_t h1 = std::hash<decltype(indentity.token)>{}(indentity.token);
			std::size_t h2 = ke::FastHashCharSequence(indentity.str.data(), indentity.str.length());
			return h1 ^ (h2 << 1);
		}
	};

	struct Equal
	{
		bool operator ()(const ActionUserDataIdentity& left, const ActionUserDataIdentity& right) const noexcept
		{
			return left.token == right.token;
		}
	};

	IdentityToken_t* token;
	std::string_view str;
};

class ActionsManager
{
	using ActionsContanier = ke::HashSet<nb_action_ptr, ke::PointerPolicy<nb_action>>;
	using UserDataMap = std::unordered_map<std::string_view, void*, ActionUserData::Hash, ActionUserData::Equal>;
	using UserDataIdentityMap = std::unordered_map<ActionUserDataIdentity, void*, ActionUserDataIdentity::Hash, ActionUserDataIdentity::Equal>;

public:
	ActionsManager();
	~ActionsManager();

	void Add(nb_action_ptr const action);
	void Remove(nb_action_ptr const action);

	inline void AddPending(nb_action_ptr const action);
	inline void RemovePending(nb_action_ptr const action) noexcept;

	inline void SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, void* data);
	inline bool GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, void*& data) const; 
	void ClearUserDataIdentity(IPluginContext* ctx);

	inline void SetUserData(nb_action_ptr action, std::string_view str, void* data); 
	inline bool GetUserData(nb_action_ptr action, std::string_view str, void*& data) const;
	void ClearUserData(nb_action_ptr action);

	void ProcessResult(nb_action_ptr action, const ActionResult<CBaseEntity>& result);
	void ProcessInitialContainedAction(const ResultType& pl, nb_action_ptr parent, nb_action_ptr oldaction, nb_action_ptr newaction);

	inline void PushRuntimeResult(std::any result);
	inline void PopRuntimeResult() noexcept;
	inline std::optional<std::reference_wrapper<std::any>> TopRuntimeResult() noexcept;

	inline ActionResult<CBaseEntity>* GetActionRuntimeResult();
	inline EventDesiredResult<CBaseEntity>* GetActionRuntimeDesiredResult();

	inline void SetRuntimeActor(CBaseEntity* actor) noexcept;
	inline CBaseEntity* GetRuntimeActor() const noexcept;

	bool GetEntityActions(CBaseEntity* entity, std::vector<nb_action_ptr>& actions);
	cell_t GetActionActorEntIndex(nb_action_ptr action);
	CBaseEntity* GetActionActor(nb_action_ptr action);

	inline bool IsValidAction(nb_action_ptr action);
	inline bool IsPending(nb_action_ptr action);

protected:
	virtual void OnActionCreated(nb_action_ptr action);
	virtual void OnActionDestroyed(nb_action_ptr action);

	bool AddEntityAction(CBaseEntity* entity, nb_action_ptr const action);
	bool RemoveEntityAction(CBaseEntity* entity, nb_action_ptr const action);
	bool RemoveEntityActions(CBaseEntity* entity);

private:
	ActionsContanier m_actions;
	std::unordered_set<nb_action_ptr> m_actionsPending;
	std::unordered_map<CBaseEntity*, std::unordered_set<nb_action_ptr>> m_entityActions;

	std::unordered_map<nb_action_ptr, UserDataMap> m_actionsUserData;
	std::unordered_map<nb_action_ptr, UserDataIdentityMap> m_actionsIndentityUserData;

	CBaseEntity* m_pRuntimeActor;
	std::stack<std::any> m_runtimeResult;
};

inline bool ActionsManager::IsValidAction(nb_action_ptr action)
{
	if (!action)
		return false;

	if (IsPending(action))
		return true;

	return m_actions.find(action).found();
}


inline void ActionsManager::SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, void* data)
{
	m_actionsIndentityUserData[action][token] = data;
}

inline bool ActionsManager::GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, void*& data) const
{
	try
	{
		data = m_actionsIndentityUserData.at(action).at(token);
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

inline void ActionsManager::SetUserData(nb_action_ptr action, std::string_view str, void* data)
{
	m_actionsUserData[action][str] = data;
}

inline bool ActionsManager::GetUserData(nb_action_ptr action, std::string_view str, void*& data) const
{
	try
	{
		data = m_actionsUserData.at(action).at(str);
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	} 
}

inline bool ActionsManager::IsPending(nb_action_ptr action)
{
	return m_actionsPending.find(action) != m_actionsPending.cend();
}

inline void ActionsManager::AddPending(nb_action_ptr const action)
{
	m_actionsPending.insert(action);
}

inline void ActionsManager::RemovePending(nb_action_ptr const action) noexcept
{
	m_actionsPending.erase(action);
}

inline void ActionsManager::PushRuntimeResult(std::any result)
{
	m_runtimeResult.push(result);
}

inline void ActionsManager::PopRuntimeResult() noexcept
{
	m_runtimeResult.pop();
}

inline std::optional<std::reference_wrapper<std::any>> ActionsManager::TopRuntimeResult() noexcept
{
	if (m_runtimeResult.empty())
		return {};

	return m_runtimeResult.top();
}

inline ActionResult<CBaseEntity>* ActionsManager::GetActionRuntimeResult()
{
	auto result = TopRuntimeResult();

	if (!result.has_value())
		return nullptr;

	try
	{
		return std::any_cast<ActionResult<CBaseEntity>*>(result->get());
	}
	catch (...)
	{
		return (ActionResult<CBaseEntity>*)GetActionRuntimeDesiredResult();
	}
}

inline EventDesiredResult<CBaseEntity>* ActionsManager::GetActionRuntimeDesiredResult()
{
	auto result = TopRuntimeResult();

	if (!result.has_value())
		return nullptr;

	try
	{
		return std::any_cast<EventDesiredResult<CBaseEntity>*>(result->get());
	}
	catch (...)
	{
		return nullptr;
	}
}

inline void ActionsManager::SetRuntimeActor(CBaseEntity* actor) noexcept
{
	m_pRuntimeActor = actor;
}

inline CBaseEntity* ActionsManager::GetRuntimeActor() const noexcept
{
	return m_pRuntimeActor;
}

extern ActionsManager g_actionsManager;

#endif // !_INCLUDE_ACTIONS_MANAGER_H