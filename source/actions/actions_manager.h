#pragma once

#include "utils.h"

#include <am-hashmap.h>
#include <vector>

#include "NextBotBehavior.h"

/* 	
	I don't know why but compile for l4d1 throws error if i use am-deque.h
	error: call to 'swap' is ambiguous
   	#include <am-deque.h>
*/

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
	//using ActionsQueque = std::deque<ActionsManager::Action*>; 
	using ActionsQueque = std::vector<ActionsManager::Action*>;
	using Actions = ke::HashMap<cell_t, ActionsQueque, IntegerPolicy>;

	bool Add(cell_t entity, Action* action);
	bool Add(CBaseEntity* entity, Action* action);
	bool Add(Action* action);

	bool Remove(cell_t entity, Action* action);
	bool Remove(CBaseEntity* entity, Action* action);
	bool Remove(Action* action);

	size_t GetEntityActions(cell_t entity, std::vector<Action*>* actions = NULL);
	bool IsValid(Action* action) const;

	void SetRuntimeAction(Action* action) noexcept;
	Action* GetRuntimeAction() const noexcept;

private:
	static void OnActionAdded(Action* action);
	static void OnActionDestroyed(Action* action);

	NODISCARD bool IsCaptured(cell_t entity, Action* action) const;
	NODISCARD bool IsCaptured(Action* action) const;
	NODISCARD bool IsCaptured(cell_t entity) const;

private:
	mutable Actions m_actions;
	bool m_init;

	Action* m_pRuntimeAction;
};

extern ActionsManager* g_pActionsManager;