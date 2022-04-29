#include "extension.h"

#include "actions_manager.h"
#include "actions_propagate.h"

ActionsManager g_ActionsManager;
ActionsManager* g_pActionsManager = &g_ActionsManager;

ActionsManager::ActionsManager() : m_pRuntimeAction(NULL)
{
	m_init = m_actions.init();

	if (!m_init)
	{
		LOGERROR("Failed to init ActionsManager");
	}
}

ActionsManager::~ActionsManager()
{
	if (!m_init)
		return;

	m_actions.clear();
}

bool ActionsManager::Add(cell_t entity, Action* action)
{
	if (IsCaptured(action))
		return false;

	auto i = m_actions.findForAdd(entity);

	if (!i.found())
		m_actions.add(i, entity, ActionsQueque());

	if (IsCaptured(entity, action))
		return false;

	DEBUG("ActionsManager::Add -> %s", action->GetName());
	i->value.push_back(action);
	ActionsManager::OnActionAdded(action);
	return true;
}

bool ActionsManager::Add(CBaseEntity* pEntity, Action* action)
{
	const cell_t entity = gamehelpers->EntityToBCompatRef(pEntity);

	if (entity == -1)
		return false;

	return Add(entity, action);
}

bool ActionsManager::Add(Action* action)
{
	return Add(static_cast<CBaseEntity*>(action->GetActor()), action);
}

bool ActionsManager::Remove(cell_t entity, Action* action)
{
	auto r = m_actions.find(entity);

	if (!r.found())
		return false;

	ActionsQueque& queque = r->value;

	for (auto iter = queque.begin(); iter != queque.end(); iter++)
	{
		if (*iter == action)
		{
			queque.erase(iter);
			DEBUG("ActionsManager::Remove -> %s", action->GetName());
			ActionsManager::OnActionDestroyed(action);
			return true;
		}
	}

	return false;
}

bool ActionsManager::Remove(CBaseEntity* pEntity, Action* action)
{
	const cell_t entity = gamehelpers->EntityToBCompatRef(pEntity);

	if (entity == -1)
		return false;

	return Remove(entity, action);
}

bool ActionsManager::Remove(Action* action)
{
	auto iterator = m_actions.iter();

	while (!iterator.empty())
	{
		if (Remove(iterator->key, action))
			return true;

		iterator.next();
	}

	return false;
}

bool ActionsManager::IsCaptured(cell_t entity, Action* action) const
{
	auto lookup = m_actions.find(entity);
	
	if (!lookup.found())
		return false;

	ActionsQueque& queque = lookup->value;

	for (auto q = queque.begin(); q != queque.end(); q++)
	{
		if (*q != action)
			continue;

		return true;
	}

	return false;
}

bool ActionsManager::IsCaptured(Action* action) const
{
	auto iter = m_actions.iter();
	
	while(!iter.empty())
	{
		if (IsCaptured(iter->key, action))
			return true;

		iter.next();
	}

	return false;
}

bool ActionsManager::IsCaptured(cell_t entity) const
{
	const auto lookup = m_actions.find(entity);
	return lookup.found();
}

size_t ActionsManager::GetEntityActions(cell_t entity, std::vector<Action*>* actions)
{
	auto r = m_actions.find(entity);

	if (!r.found())
		return 0;

	auto& quequ = r->value;
	size_t count = 0;

	for (auto action : quequ)
	{
		if (actions)
			actions->push_back(action);

		count++;
	}

	return count;
}

bool ActionsManager::IsValid(Action* action) const
{
#ifdef NO_RUNTIME_VALIDATE_CHECK
	return true;
#else
	if (action == NULL)
		return false;
	
	return IsCaptured(action) || GetRuntimeAction() == action;
#endif
}

void ActionsManager::SetRuntimeAction(Action* action) noexcept
{
	m_pRuntimeAction = action;
}

Action<void>* ActionsManager::GetRuntimeAction() const noexcept
{
	return m_pRuntimeAction;
}

void ActionsManager::OnActionAdded(Action* action)
{
	static IForward* forward = forwards->CreateForward("OnActionCreated", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);

	ActionsPropagate::OnActionAdded(action);

	forward->PushCell((cell_t)action);
	forward->PushCell((cell_t)action->GetActor());
	forward->PushString(action->GetName());
	forward->Execute();
}

void ActionsManager::OnActionDestroyed(Action* action)
{
	static IForward* forward = forwards->CreateForward("OnActionDestroyed", ET_Ignore, 3, NULL, Param_Cell, Param_Cell, Param_String);

	ActionsPropagate::OnActionDestroyed(action);

	forward->PushCell((cell_t)action);
	forward->PushCell((cell_t)action->GetActor());
	forward->PushString(action->GetName());
	forward->Execute();
}