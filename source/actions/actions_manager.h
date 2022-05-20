#pragma once

#include "utils.h"

#include <am-hashmap.h>
#include <am-deque.h>

#include "NextBotBehavior.h"

class ActionsManager;

struct IntegerPolicy
{
	static inline uint32_t hash(cell_t i)
	{
		return ke::HashInteger<sizeof(cell_t)>(i);
	}

	static inline bool matches(cell_t i1, cell_t i2) noexcept
	{
		return i1 == i2;
	}
};

class ActionsManager
{
public:
	ActionsManager();
	~ActionsManager();

	using Action = Action<void>;
	using ActionsQueque = std::deque<ActionsManager::Action*>; 
	//using ActionsQueque = std::vector<ActionsManager::Action*>;
	using Actions = ke::HashMap<cell_t, ActionsQueque, IntegerPolicy>;

	using iterator = ActionsQueque::iterator;
	using citerator = ActionsQueque::const_iterator;

	bool Add(cell_t entity, Action* action);
	bool Add(CBaseEntity* entity, Action* action);
	bool Add(Action* action);

	bool Remove(cell_t entity, Action* action);
	bool Remove(CBaseEntity* entity, Action* action);
	bool Remove(Action* action);

	size_t GetEntityActions(cell_t entity, std::vector<Action*>* actions = NULL);
	
	bool AddPending(Action* action);
	bool RemovePending(Action* action);

	bool IsPending(Action* action, bool erase = false) const;

	bool IsValidAction(Action* action) const;
	bool IsValidResult(const void* const result) const;

	void SetRuntimeAction(Action* action) noexcept;
	Action* GetRuntimeAction() const noexcept;

	void SetRuntimeResult(void* const result) noexcept;
	void* const GetRuntimeResult() const noexcept;

	void SetRuntimeActor(CBaseEntity* actor) noexcept;
	CBaseEntity* GetRuntimeActor() const noexcept;

private:
	static void OnActionAdded(Action* action);
	static void OnActionDestroyed(Action* action);

	NODISCARD bool IsCaptured(cell_t entity, Action* action) const;
	NODISCARD bool IsCaptured(Action* action) const;
	NODISCARD bool IsCaptured(cell_t entity) const;

private:
	bool m_init;

	mutable Actions m_actions;
	mutable ActionsQueque m_pendingActions;

	CBaseEntity* m_pRuntimeActor;
	Action* m_pRuntimeAction;
	void* m_pRuntimeResult;
};

extern ActionsManager* g_pActionsManager;