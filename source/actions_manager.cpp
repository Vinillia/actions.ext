#include "actions_manager.h"
#include "actions_processor.h"
#include "extension.h"

extern ConVar ext_actions_debug;
ActionsManager g_actionsManager;

ActionsManager::ActionsManager()
{
	m_actions.init();
	m_pRuntimeActor = nullptr;
}

ActionsManager::~ActionsManager()
{

}

void ActionsManager::Add(nb_action_ptr const action)
{
	m_actions.add(action);
	OnActionCreated(action);
}

void ActionsManager::Remove(nb_action_ptr const action)
{
	OnActionDestroyed(action);
	m_actions.removeIfExists(action);
}

void ActionsManager::ClearUserDataIdentity(IPluginContext* ctx)
{
	for (auto udIter : m_actionsIndentityUserData)
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

CBaseEntity* ActionsManager::GetActionActor(nb_action_ptr action)
{
	CBaseEntity* actor = action->GetActor();

	if (actor == nullptr)
		actor = GetRuntimeActor();

	return actor;
}

cell_t ActionsManager::GetActionActorEntIndex(nb_action_ptr action)
{
	CBaseEntity* actor = action->GetActor();
	cell_t entindex = -1;

	if (actor == nullptr || (entindex = gamehelpers->EntityToBCompatRef(actor)) == -1)
	{
		actor = GetRuntimeActor();

		if (actor != nullptr)
			entindex = gamehelpers->EntityToBCompatRef(actor);
	}

	if (entindex == -1)
		return 0;

	return entindex;
}

void ActionsManager::ProcessResult(nb_action_ptr action, const ActionResult<CBaseEntity>& result)
{
	if (!result.IsRequestingChange())
		return;

	if (ext_actions_debug.GetBool())
	{
		cell_t actor = g_actionsManager.GetActionActorEntIndex(action);
		const char* classname = gamehelpers->GetEntityClassname(g_actionsManager.GetActionActor(action));

		if (result.IsDone())
		{
			if (action->GetActionBuriedUnderMe())
				Msg("%i(%s): %s is done because '%s'. Continue suspended action %s.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN", action->GetActionBuriedUnderMe()->GetName());
			else
				Msg("%i(%s): %s is done because '%s'.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else if (result.m_type == SUSPEND_FOR)
		{
			Msg("%i(%s): %s suspended for %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else
		{
			Msg("%i(%s): %s changed to %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
	}

	if (!result.IsDone())
	{
		SetRuntimeActor(action->GetActor());
		Add(result.m_action);
	}

	// This is to early to remove action
	// game still not invoked OnEnd call  
	// if (result.m_type != SUSPEND_FOR)
	//	Remove(action);
}

void ActionsManager::ProcessInitialContainedAction(const ResultType& pl, nb_action_ptr parent, nb_action_ptr oldaction, nb_action_ptr newaction)
{
	if (pl == Pl_Continue)
	{
		if (oldaction)
		{
			SetRuntimeActor(parent->GetActor());
			Add(oldaction);
		}

		if (newaction && newaction != oldaction)
		{
			Warning("Tried to override initial action with Plugin_Continue: %s\n", newaction->GetName());
			delete newaction;
		}

		return;
	}

	if (newaction)
	{
		if (oldaction)
			delete oldaction;

		Add(newaction);
	}
}

void ActionsManager::OnActionCreated(nb_action_ptr action)
{
	if (IsPending(action))
		RemovePending(action);

	BeginActionProcessing(action);
	AddEntityAction(GetActionActor(action), action);
	g_sdkActions.OnActionCreated(action);
}

void ActionsManager::OnActionDestroyed(nb_action_ptr action)
{
	g_sdkActions.OnActionDestroyed(action);
	RemoveEntityAction(GetActionActor(action), action);
	StopActionProcessing(action);
	RemovePending(action);
	ClearUserData(action);

	SetRuntimeActor(nullptr);
}

bool ActionsManager::GetEntityActions(CBaseEntity* entity, std::vector<nb_action_ptr>& vec)
{
	for (auto& action : m_entityActions[entity])
	{
		vec.push_back(action);
	}

	return true;
}

bool ActionsManager::AddEntityAction(CBaseEntity* entity, nb_action_ptr const action)
{
	m_entityActions[entity].insert(action);
	return true;
}

bool ActionsManager::RemoveEntityAction(CBaseEntity* entity, nb_action_ptr const action)
{
	m_entityActions[entity].erase(action);
	return true;
}

bool ActionsManager::RemoveEntityActions(CBaseEntity* entity)
{
	m_entityActions[entity].clear();
	return true;
}