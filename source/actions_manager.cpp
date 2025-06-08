#include "actions_manager.h"
#include "actions_processor_impl.h"
#include "extension.h"

extern ConVar ext_actions_debug;
extern ConVar ext_actions_debug_cache;
ActionsManager g_actionsManager;

ActionsManager::ActionsManager()
{
	m_entityActions.resize(MAX_EDICTS);
	m_actions.init();
}

ActionsManager::~ActionsManager()
{

}

void ActionsManager::Add(nb_action_ptr const action)
{
	if (m_actions.find(action).found())
		return;

	m_actions.add(action);
	OnActionCreated(action);
}

void ActionsManager::Remove(nb_action_ptr const action)
{
	if (!m_actions.find(action).found())
		return;

	OnActionDestroyed(action);
	m_actions.removeIfExists(action);
}

void ActionsManager::ClearUserDataIdentity(IPluginContext* ctx)
{
	for (auto& udIter : m_actionsIndentityUserData)
	{
		auto& identmap = udIter.second;

		for (auto idIter = identmap.begin(); idIter != identmap.end();)
		{
			if (ctx->GetIdentity() == idIter->first.token)
			{
				idIter = identmap.erase(idIter);
			}
			else
			{
				idIter++;
			}
		}
	}
}

void ActionsManager::ClearUserData(nb_action_ptr action)
{
	m_actionsUserData[action].clear();
}

cell_t ActionsManager::GetActionActorEntIndex(nb_action_ptr action)
{
	CBaseEntity* actor = action->GetActor();
	cell_t entindex = -1;

	if (actor == nullptr || (entindex = gamehelpers->EntityToBCompatRef(actor)) == -1)
	{
		actor = GetActionActor(action);

		if (actor != nullptr)
			entindex = gamehelpers->EntityToBCompatRef(actor);
	}

	if (entindex == -1)
		return 0;

	if (actor)
		entindex = gamehelpers->EntityToBCompatRef(actor);

	return entindex;
}

void ActionsManager::ProcessResult(nb_action_ptr action, const ActionResult<CBaseEntity>& result)
{
	if (!result.IsRequestingChange())
		return;

	if (!result.IsDone())
	{
		SetActionActor(result.m_action, action->GetActor());
		Add(result.m_action);
	}

	if (ext_actions_debug.GetBool())
	{
		cell_t actor = g_actionsManager.GetActionActorEntIndex(action);
		const char* classname = gamehelpers->GetEntityClassname(g_actionsManager.GetActionActor(action));

		if (result.IsDone())
		{
			if (action->GetActionBuriedUnderMe())
				MsgSM("%i(%s): %s has done because '%s'. Continue suspended action '%s'.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN", action->GetActionBuriedUnderMe()->GetName());
			else
				MsgSM("%i(%s): %s has done because '%s'.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else if (result.m_type == SUSPEND_FOR)
		{
			MsgSM("%i(%s): %s suspended for %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else
		{
			MsgSM("%i(%s): %s changed to %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
	}
}

void ActionsManager::ProcessInitialContainedAction(const ResultType& pl, nb_action_ptr parent, nb_action_ptr oldaction, nb_action_ptr newaction)
{
	if (pl == Pl_Continue)
	{
		if (oldaction)
		{
			if (parent)
				g_actionsManager.SetActionActor(oldaction, parent->GetActor());

			Add(oldaction);
		}

		if (newaction && newaction != oldaction)
		{
			WarningSM("Tried to override initial action with Plugin_Continue: %s\n", newaction->GetName());
			delete newaction;
		}

		return;
	}

	if (newaction)
	{
		if (oldaction)
			delete oldaction;

		if (parent)
			g_actionsManager.SetActionActor(newaction, parent->GetActor());
		
		Add(newaction);
	}
}

void ActionsManager::OnActionCreated(nb_action_ptr action)
{
	if (IsPending(action))
		RemovePending(action);

	BeginActionProcessing(action);
	
	ActionId id = RegisterActionID(action->GetName());

	StoreEntityAction(action, id);
	g_sdkActions.OnActionCreated(action, id);
}

void ActionsManager::OnActionDestroyed(nb_action_ptr action)
{
	ActionId id = FindActionID(action->GetName());

	g_sdkActions.OnActionDestroyed(action, id);
	DeleteEntityAction(action, id);
	StopActionProcessing(action);
	RemovePending(action);
	ClearUserData(action);
}

void ActionsManager::StoreEntityAction(nb_action_ptr action, ActionId id)
{
	cell_t index = GetActionActorEntIndex(action);

	if (index >= static_cast<cell_t>(m_entityActions.size()))
	{
		m_entityActions.resize(index);
	}

	m_entityActions[index].emplace(id, action);
	
	if (ext_actions_debug_cache.GetBool())
	{
		MsgSM("%.3f:%i: Stored action %s ( %i, 0x%X )", gpGlobals->curtime, index, action->GetName(), id, action);
	}
}

void ActionsManager::DeleteEntityAction(nb_action_ptr action, ActionId action_id)
{
	cell_t index = GetActionActorEntIndex(action);

	if (index >= static_cast<cell_t>(m_entityActions.size()))
		return;

	auto& map = m_entityActions[index];
	for (auto it = map.find(action_id); it != map.end() && it->first == action_id; ) 
	{
		if (it->second == action)
		{
			it = map.erase(it);
			break;
		}
		else 
		{
			++it;
		}
	}
}

ActionsManager::ActionId ActionsManager::RegisterActionID(std::string_view name)
{
	auto it = m_actionIDs.find(name.data());
	if (it != m_actionIDs.end())
		return it->second;

	ActionId& id = m_actionIDs[name.data()];
	id = global_action_id++;

	if (ext_actions_debug_cache.GetBool())
	{
		MsgSM("Registered ActionId %s ( %i )", name.data(), id);
	}

	return id;
}

ActionsManager::ActionId ActionsManager::FindActionID(std::string_view name) const
{
	auto it = m_actionIDs.find(name.data());
	if (it != m_actionIDs.end())
		return it->second;

	return null_action_id;
}

nb_action_ptr ActionsManager::LookupEntityAction(CBaseEntity* entity, ActionId id) const
{
	if (id == null_action_id)
		return nullptr;

	cell_t index = gamehelpers->EntityToBCompatRef(entity);

	if (index == -1)
		return nullptr;

	if (index >= static_cast<cell_t>(m_entityActions.size()))
		return nullptr;

	auto range = m_entityActions[index].equal_range(id);
	for (auto it = range.first; it != range.second; ++it) 
	{
		return it->second;
	}

	return nullptr;
}

nb_action_ptr ActionsManager::LookupEntityAction(CBaseEntity* entity, const char* name) const
{
	return LookupEntityAction(entity, FindActionID(name));
}
