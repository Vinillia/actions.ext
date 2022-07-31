#include "actions_processor.h"
#include "offset_manager.h"

#if defined __linux__
	SH_DECL_MANUALHOOK0_void(Destructor, 1, 0, 0);
#else
	SH_DECL_MANUALHOOK1_void(Destructor, 0, 0, 0, bool);
#endif

SH_DECL_MANUALHOOK2(OnStart, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(Update, 0, 0, 0, ActionResult<void>, CBaseEntity*, float);
SH_DECL_MANUALHOOK2_void(OnEnd, 0, 0, 0, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(OnSuspend, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(OnResume, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK1(InitialContainedAction, 0, 0, 0, Action<void>*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnLeaveGround, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnLandOnGround, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK3(OnContact, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CGameTrace*);
SH_DECL_MANUALHOOK2(OnMoveToSuccess, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const Path*);
SH_DECL_MANUALHOOK3(OnMoveToFailure, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const Path*, MoveToFailureType);
SH_DECL_MANUALHOOK1(OnStuck, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK1(OnUnStuck, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK1(OnPostureChanged, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnAnimationActivityComplete, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, int);
SH_DECL_MANUALHOOK2(OnAnimationActivityInterrupted, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, int);
SH_DECL_MANUALHOOK2(OnAnimationEvent, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, animevent_t*);
SH_DECL_MANUALHOOK1(OnIgnite, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnInjured, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
SH_DECL_MANUALHOOK2(OnKilled, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
SH_DECL_MANUALHOOK3(OnOtherKilled, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const CTakeDamageInfo*);
SH_DECL_MANUALHOOK2(OnSight, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnLostSight, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnThreatChanged, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK4(OnSound, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const Vector&, KeyValues*);
SH_DECL_MANUALHOOK5(OnSpokeConcept, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseCombatCharacter*, AIConcept_t, AI_Response*, void*);
SH_DECL_MANUALHOOK3(OnNavAreaChanged, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CNavArea*, CNavArea*);
SH_DECL_MANUALHOOK1(OnModelChanged, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK3(OnPickUp, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnDrop, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnShoved, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnBlinded, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK1(OnEnteredSpit, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnHitByVomitJar, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnCommandAttack, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK1(OnCommandAssault, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK3(OnCommandApproachVector, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const Vector&, float);
SH_DECL_MANUALHOOK2(OnCommandApproachEntity, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
SH_DECL_MANUALHOOK3(OnCommandRetreat, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, float);
SH_DECL_MANUALHOOK2(OnCommandPause, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, float);
SH_DECL_MANUALHOOK1(OnCommandResume, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*);
SH_DECL_MANUALHOOK2(OnCommandString, 0, 0, 0, EventDesiredResult<void>, CBaseEntity*, const char*);
SH_DECL_MANUALHOOK1(IsAbleToBlockMovementOf, 0, 0, 0, bool, const INextBot*);

const std::map<std::string, int32_t>* g_CachedOffsets;

ActionProcessor::ActionProcessor(CBaseEntity* entity, Action<void>* action) : m_action(action)
{
	g_pActionsManager->SetRuntimeActor(entity);
	g_pActionsManager->Add(entity, action);

	static std::vector<std::string> g_hookedNames;

	for (auto name : g_hookedNames)
	{
		if (strcmp(name.c_str(), action->GetName()) == 0)
			return;
	}

	g_hookedNames.push_back(action->GetName());
	
	START_PROCESSOR(Destructor, dctor);
	START_PROCESSOR(OnStart, start);
	START_PROCESSOR(Update, update);
	START_PROCESSOR(OnEnd, end);
	START_PROCESSOR(OnSuspend, suspend);
	START_PROCESSOR(OnResume, resume);
	START_PROCESSOR(InitialContainedAction, initialAction);
	START_PROCESSOR(OnLeaveGround, leaveGround);
	START_PROCESSOR(OnLandOnGround, landGround);
	START_PROCESSOR(OnContact, contact);
	START_PROCESSOR(OnMoveToSuccess, movetoSuccess);
	START_PROCESSOR(OnMoveToFailure, movetoFailure);
	START_PROCESSOR(OnStuck, stuck);
	START_PROCESSOR(OnUnStuck, unstuck);
	START_PROCESSOR(OnPostureChanged, postureChanged);
	START_PROCESSOR(OnAnimationActivityComplete, animationActivityComplete);
	START_PROCESSOR(OnAnimationActivityInterrupted, animationActivityInterrupted);
	START_PROCESSOR(OnAnimationEvent, animationEvent);
	START_PROCESSOR(OnIgnite, ignite);
	START_PROCESSOR(OnInjured, injured);
	START_PROCESSOR(OnKilled, killed);
	START_PROCESSOR(OnOtherKilled, otherKilled);
	START_PROCESSOR(OnSight, sight);
	START_PROCESSOR(OnLostSight, lostSight);
	START_PROCESSOR(OnThreatChanged, threatChanged);
	START_PROCESSOR(OnSound, sound);
	START_PROCESSOR(OnSpokeConcept, spokeConcept);
	START_PROCESSOR(OnNavAreaChanged, navareaChanged);
	START_PROCESSOR(OnModelChanged, modelChanged);
	START_PROCESSOR(OnPickUp, pickup);
	START_PROCESSOR(OnDrop, drop);
	START_PROCESSOR(OnShoved, shoved);
	START_PROCESSOR(OnBlinded, blinded);
	START_PROCESSOR(OnCommandAttack, commandAttack);
	START_PROCESSOR(OnCommandApproachVector, commandApproachVector);
	START_PROCESSOR(OnCommandApproachEntity, commandApproachEntity);
	START_PROCESSOR(OnCommandRetreat, commandRetreat);
	START_PROCESSOR(OnCommandPause, commandPause);
	START_PROCESSOR(OnCommandResume, commandResume);
	START_PROCESSOR(IsAbleToBlockMovementOf, abletoBlock);
	START_PROCESSOR(OnCommandAssault, commandAssault);
	START_PROCESSOR(OnEnteredSpit, enteredSpit);
	START_PROCESSOR(OnHitByVomitJar, hitVomitjar);
	START_PROCESSOR(OnCommandString, commandString);
}

ActionProcessor::ActionProcessor(Action<void>* action) : ActionProcessor(static_cast<CBaseEntity*>(action->GetActor()), action)
{
}

bool ConfigureHooks()
{
	g_CachedOffsets = &GetOffsetsManager()->GetRequestedOffsets();

	RECONFIGURE_MANUALHOOK(Destructor);
	RECONFIGURE_MANUALHOOK(OnStart);
	RECONFIGURE_MANUALHOOK(Update);
	RECONFIGURE_MANUALHOOK(OnEnd);
	RECONFIGURE_MANUALHOOK(OnSuspend);
	RECONFIGURE_MANUALHOOK(OnResume);
	RECONFIGURE_MANUALHOOK(InitialContainedAction);
	RECONFIGURE_MANUALHOOK(OnLeaveGround);
	RECONFIGURE_MANUALHOOK(OnLandOnGround);
	RECONFIGURE_MANUALHOOK(OnContact);
	RECONFIGURE_MANUALHOOK(OnMoveToSuccess);
	RECONFIGURE_MANUALHOOK(OnMoveToFailure);
	RECONFIGURE_MANUALHOOK(OnStuck);
	RECONFIGURE_MANUALHOOK(OnUnStuck);
	RECONFIGURE_MANUALHOOK(OnPostureChanged);
	RECONFIGURE_MANUALHOOK(OnAnimationActivityComplete);
	RECONFIGURE_MANUALHOOK(OnAnimationActivityInterrupted);
	RECONFIGURE_MANUALHOOK(OnAnimationEvent);
	RECONFIGURE_MANUALHOOK(OnIgnite);
	RECONFIGURE_MANUALHOOK(OnInjured);
	RECONFIGURE_MANUALHOOK(OnKilled);
	RECONFIGURE_MANUALHOOK(OnOtherKilled);
	RECONFIGURE_MANUALHOOK(OnSight);
	RECONFIGURE_MANUALHOOK(OnLostSight);
	RECONFIGURE_MANUALHOOK(OnThreatChanged);
	RECONFIGURE_MANUALHOOK(OnSound);
	RECONFIGURE_MANUALHOOK(OnSpokeConcept);
	RECONFIGURE_MANUALHOOK(OnNavAreaChanged);
	RECONFIGURE_MANUALHOOK(OnModelChanged);
	RECONFIGURE_MANUALHOOK(OnPickUp);
	RECONFIGURE_MANUALHOOK(OnDrop);
	RECONFIGURE_MANUALHOOK(OnShoved);
	RECONFIGURE_MANUALHOOK(OnBlinded);
	RECONFIGURE_MANUALHOOK(OnCommandApproachVector);
	RECONFIGURE_MANUALHOOK(OnCommandAttack);
	RECONFIGURE_MANUALHOOK(OnCommandRetreat);
	RECONFIGURE_MANUALHOOK(OnCommandPause);
	RECONFIGURE_MANUALHOOK(OnCommandResume);
	RECONFIGURE_MANUALHOOK(OnEnteredSpit);
	RECONFIGURE_MANUALHOOK(OnHitByVomitJar);
	RECONFIGURE_MANUALHOOK(OnCommandAssault);
	RECONFIGURE_MANUALHOOK(OnCommandString);
	RECONFIGURE_MANUALHOOK(IsAbleToBlockMovementOf);
	RECONFIGURE_MANUALHOOK(OnCommandApproachEntity);

	return !GetOffsetsManager()->HaveFailedRequest();
}

OffsetManager* GetOffsetsManager()
{
	static OffsetManager offsmgr(CONFIG_FILE_NAME);
	return &offsmgr;
}

template<typename T>
void CheckActionResult(Action<void>* action, T& result)
{
	if (!result.IsRequestingChange())
		return;

	if (ext_actions_debug.GetBool())
	{
		cell_t actor = gamehelpers->EntityToBCompatRef((CBaseEntity*)action->GetActor());
		const char* classname = gamehelpers->GetEntityClassname((CBaseEntity*)action->GetActor());

		if(result.IsDone())
		{
			if (action->GetActionBuriedUnderMe())
				LOG("%i(%s): %s is done because '%s'. Continue suspended action %s.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN", action->GetActionBuriedUnderMe()->GetName());
			else
				LOG("%i(%s): %s is done because '%s'.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else if (result.m_type == SUSPEND_FOR)
		{
			LOG("%i(%s): %s suspended for %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else
		{
			LOG("%i(%s): %s changed to %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
	}

	if (!result.IsDone())
		ExecuteContextualProcessor(static_cast<CBaseEntity*>(action->GetActor()), result.m_action);

	if (result.m_type != SUSPEND_FOR)
		g_pActionsManager->Remove(action);
}

void ExecuteProcessor(CBaseEntity* entity, Action<void>* action)
{
	ActionProcessor processor(entity, action);
}