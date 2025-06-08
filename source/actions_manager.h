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
#include <stdexcept>

#include <NextBotBehavior.h>

template<typename T>
struct ActionResult;

template<typename T>
struct EventDesiredResult;

struct ActionUserData
{
	~ActionUserData() = default;

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

	
	// union
	// {
	cell_t data;
	Vector vector;
	std::string string;
	// };
};

struct ActionUserDataIdentity
{
	struct Hash
	{
		std::size_t operator ()(const ActionUserDataIdentity& indentity) const noexcept
		{
			std::size_t h1 = std::hash<decltype(indentity.token)>{}(indentity.token);
			std::size_t h2 = ke::FastHashCharSequence(indentity.key.data(), indentity.key.length());
			return h1 ^ (h2 << 1);
		}
	};

	struct Equal
	{
		bool operator ()(const ActionUserDataIdentity& left, const ActionUserDataIdentity& right) const noexcept
		{
			return left.token == right.token && left.key.compare(right.key) == 0;
		}
	};

	IdentityToken_t* token;
	std::string_view key;
};

class ActionsManager
{
private:
	using ActionsContanier = ke::HashSet<nb_action_ptr, ke::PointerPolicy<nb_action>>;
	using UserDataMap = std::unordered_map<std::string_view, ActionUserData, ActionUserData::Hash, ActionUserData::Equal>;
	using UserDataIdentityMap = std::unordered_map<ActionUserDataIdentity, ActionUserData, ActionUserDataIdentity::Hash, ActionUserDataIdentity::Equal>;

public:
	using ActionId = int32_t;
	static inline constexpr ActionId null_action_id = 0;

private:
	// there are extremly rare cases when the same multiple actions can be registered for the same entity besides user's intervention
	using entity_action_map = std::unordered_multimap<ActionId, nb_action_ptr>;

	static inline ActionId global_action_id = 1;

public:
	ActionsManager();
	~ActionsManager();

	inline void Add(nb_action_ptr const action, CBaseEntity* const entity);

	void Add(nb_action_ptr const action);
	void Remove(nb_action_ptr const action);

	inline void AddPending(nb_action_ptr const action);
	inline void RemovePending(nb_action_ptr const action) noexcept;

	inline void SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, cell_t data);
	inline void SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, const Vector& vector);
	inline void SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, std::string_view string);

	inline bool GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, cell_t& data) const;
	inline bool GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, Vector& vector) const;
	inline bool GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, std::string& string) const;

	void ClearUserDataIdentity(IPluginContext* ctx);

	inline void SetUserData(nb_action_ptr action, std::string_view str, cell_t data);
	inline void SetUserData(nb_action_ptr action, std::string_view str, const Vector& vector);
	inline void SetUserData(nb_action_ptr action, std::string_view str, std::string_view string);
	
	inline bool GetUserData(nb_action_ptr action, std::string_view str, cell_t& data) const;
	inline bool GetUserData(nb_action_ptr action, std::string_view str, Vector& vector);
	inline bool GetUserData(nb_action_ptr action, std::string_view str, std::string& string);

	void ClearUserData(nb_action_ptr action);

	void ProcessResult(nb_action_ptr action, const ActionResult<CBaseEntity>& result);
	void ProcessInitialContainedAction(const ResultType& pl, nb_action_ptr parent, nb_action_ptr oldaction, nb_action_ptr newaction);

	inline void PushRuntimeResult(std::any result);
	inline void PopRuntimeResult() noexcept;

	inline void SetRuntimeAction(nb_action_ptr action);
	inline nb_action_ptr GetRuntimeAction() const noexcept;

	ActionId RegisterActionID(std::string_view name);
	ActionId FindActionID(std::string_view name) const;

	nb_action_ptr LookupEntityAction(CBaseEntity* entity, ActionId id) const;
	nb_action_ptr LookupEntityAction(CBaseEntity* entity, const char* name) const;

	/* 
	* error: no member named 'value' in 'std::is_copy_constructible<std::reference_wrapper<std::any>>'
	* clang or whatever thinks std::any is not copy-constructible
	*/
	// inline std::optional<std::reference_wrapper<std::any>> TopRuntimeResult() noexcept;

	inline std::any& TopRuntimeResult();

	inline ActionResult<CBaseEntity>* GetActionRuntimeResult();
	inline EventDesiredResult<CBaseEntity>* GetActionRuntimeDesiredResult();

	inline void SetActionActor(nb_action_ptr action, CBaseEntity* actor) noexcept;
	inline CBaseEntity* GetActionActor(nb_action_ptr action) const noexcept;

	cell_t GetActionActorEntIndex(nb_action_ptr action);

	inline bool IsValidAction(nb_action_ptr action);
	inline bool IsPending(nb_action_ptr action);

protected:
	virtual void OnActionCreated(nb_action_ptr action);
	virtual void OnActionDestroyed(nb_action_ptr action);

	void StoreEntityAction(nb_action_ptr action, ActionId id);
	void DeleteEntityAction(nb_action_ptr action, ActionId id);

private:
	ActionsContanier m_actions;
	std::unordered_set<nb_action_ptr> m_actionsPending;
	std::unordered_map<nb_action_ptr, CBaseEntity*> m_actionActor;

	std::unordered_map<nb_action_ptr, UserDataMap> m_actionsUserData;
	std::unordered_map<nb_action_ptr, UserDataIdentityMap> m_actionsIndentityUserData;

	std::unordered_map<std::string, ActionId> m_actionIDs;

	std::vector<entity_action_map> m_entityActions;
	
	nb_action_ptr m_pRuntimeAction;
	std::stack<std::any> m_runtimeResult;
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


inline void ActionsManager::SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, cell_t data)
{
	m_actionsIndentityUserData[action][token].data = data;
}

inline void ActionsManager::SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, const Vector& vector)
{
	m_actionsIndentityUserData[action][token].vector = vector;
}

inline void ActionsManager::SetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, std::string_view string)
{
	m_actionsIndentityUserData[action][token].string = string;
}

inline bool ActionsManager::GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, cell_t& data) const
{
	try
	{
		data = m_actionsIndentityUserData.at(action).at(token).data;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline bool ActionsManager::GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, Vector& vector) const
{
	try
	{
		vector = m_actionsIndentityUserData.at(action).at(token).vector;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline bool ActionsManager::GetUserDataIdentity(nb_action_ptr action, const ActionUserDataIdentity& token, std::string& string) const
{
	try
	{
		string = m_actionsIndentityUserData.at(action).at(token).string;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline void ActionsManager::SetUserData(nb_action_ptr action, std::string_view str, cell_t data)
{
	m_actionsUserData[action][str].data = data;
}

inline void ActionsManager::SetUserData(nb_action_ptr action, std::string_view str, const Vector& vector)
{
	m_actionsUserData[action][str].vector = vector;
}

inline void ActionsManager::SetUserData(nb_action_ptr action, std::string_view str, std::string_view string)
{
	m_actionsUserData[action][str].string = string;
}

inline bool ActionsManager::GetUserData(nb_action_ptr action, std::string_view str, cell_t& data) const
{
	try
	{
		data = m_actionsUserData.at(action).at(str).data;
		return true;
	}
	catch (...)
	{
		return false;
	} 
}

inline bool ActionsManager::GetUserData(nb_action_ptr action, std::string_view str, Vector& vector)
{
	try
	{
		vector = m_actionsUserData.at(action).at(str).vector;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline bool ActionsManager::GetUserData(nb_action_ptr action, std::string_view str, std::string& string)
{
	try
	{
		string = m_actionsUserData.at(action).at(str).string;
		return true;
	}
	catch (...)
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

inline void ActionsManager::SetRuntimeAction(nb_action_ptr action)
{
	m_pRuntimeAction = action;
}

inline nb_action_ptr ActionsManager::GetRuntimeAction() const noexcept
{
	return m_pRuntimeAction;
}

inline std::any& ActionsManager::TopRuntimeResult()
{
	if (m_runtimeResult.empty())
		throw std::runtime_error("Invalid runtime result access");

	return m_runtimeResult.top();
}

inline ActionResult<CBaseEntity>* ActionsManager::GetActionRuntimeResult()
{
	try
	{
 		std::any& result = TopRuntimeResult();
		return std::any_cast<ActionResult<CBaseEntity>*>(result);
	}
	catch (const std::runtime_error&)
	{
		return nullptr;
	}
	catch (...)
	{
		return (ActionResult<CBaseEntity>*)GetActionRuntimeDesiredResult();
	}
}

inline EventDesiredResult<CBaseEntity>* ActionsManager::GetActionRuntimeDesiredResult()
{
	try
	{
		std::any& result = TopRuntimeResult();
		return std::any_cast<EventDesiredResult<CBaseEntity>*>(result);
	}
	catch (...)
	{
		return nullptr;
	}
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
