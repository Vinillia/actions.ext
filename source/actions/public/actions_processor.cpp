#include "actions_processor.h"

#define RECONFIGURE_MANUALHOOK(hookname, vtableidx) \
		SH_MANUALHOOK_RECONFIGURE(hookname, vtableidx, 0, 0); \
		map[#hookname] = vtableidx

#if defined __linux__
	SH_DECL_MANUALHOOK0_void(OnDestroyed, 1, 0, 0);
#else
	SH_DECL_MANUALHOOK1_void(OnDestroyed, 0, 0, 0, bool);
#endif

SH_DECL_MANUALHOOK2(OnStart, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(OnUpdate, 0, 0, 0, ActionResult<void>, CBaseEntity*, float);
SH_DECL_MANUALHOOK2_void(OnEnd, 0, 0, 0, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(OnSuspend, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK2(OnResume, 0, 0, 0, ActionResult<void>, CBaseEntity*, Action<void>*);
SH_DECL_MANUALHOOK1(OnInitialContainedAction, 0, 0, 0, Action<void>*, CBaseEntity*);
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

ActionProcessor::ActionProcessor(CBaseEntity* entity, Action<void>* action) : m_action(action)
{
	g_pActionsManager->SetRuntimeActor(entity);
	g_pActionsManager->Add(entity, action);

	for (auto name : m_hookedNames)
	{
		if (strcmp(name.c_str(), action->GetName()) == 0)
			return;
	}

	std::map<std::string, size_t>& offsets = GetOffsetsInfo();
	m_hookedNames.push_back(action->GetName());

	START_PROCESSOR(OnDestroyed, dctor);
	START_PROCESSOR(OnStart, start);
	START_PROCESSOR(OnUpdate, update);
	START_PROCESSOR(OnEnd, end);
	START_PROCESSOR(OnSuspend, suspend);
	START_PROCESSOR(OnResume, resume);
	START_PROCESSOR(OnInitialContainedAction, initialAction);
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

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		START_PROCESSOR(OnCommandAssault, commandAssault);
		START_PROCESSOR(OnEnteredSpit, enteredSpit);
		START_PROCESSOR(OnHitByVomitJar, hitVomitjar);
		START_PROCESSOR(OnCommandString, commandString);
	#endif

}

ActionProcessor::ActionProcessor(Action<void>* action) : ActionProcessor(static_cast<CBaseEntity*>(action->GetActor()), action)
{
}

void ReconfigureHooks()
{
	auto& map = GetOffsetsInfo();

	#ifndef __linux__ 
		RECONFIGURE_MANUALHOOK(OnDestroyed, 0);
	#else
		RECONFIGURE_MANUALHOOK(OnDestroyed, 1);
	#endif

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		#ifndef __linux__
			constexpr size_t offset = 0;
		#else
			constexpr size_t offset = 1;
		#endif
	#else
		#ifndef __linux__
			constexpr size_t offset = -5;
		#else
			constexpr size_t offset = -4;
		#endif
	#endif

	RECONFIGURE_MANUALHOOK(OnStart, 43 + offset);
	RECONFIGURE_MANUALHOOK(OnUpdate, 44 + offset);
	RECONFIGURE_MANUALHOOK(OnEnd, 45 + offset);
	RECONFIGURE_MANUALHOOK(OnSuspend, 46 + offset);
	RECONFIGURE_MANUALHOOK(OnResume, 47 + offset);
	RECONFIGURE_MANUALHOOK(OnInitialContainedAction, 48 + offset);
	RECONFIGURE_MANUALHOOK(OnLeaveGround, 49 + offset);
	RECONFIGURE_MANUALHOOK(OnLandOnGround, 50 + offset);
	RECONFIGURE_MANUALHOOK(OnContact, 51 + offset);
	RECONFIGURE_MANUALHOOK(OnMoveToSuccess, 52 + offset);
	RECONFIGURE_MANUALHOOK(OnMoveToFailure, 53 + offset);
	RECONFIGURE_MANUALHOOK(OnStuck, 54 + offset);
	RECONFIGURE_MANUALHOOK(OnUnStuck, 55 + offset);
	RECONFIGURE_MANUALHOOK(OnPostureChanged, 56 + offset);
	RECONFIGURE_MANUALHOOK(OnAnimationActivityComplete, 57 + offset);
	RECONFIGURE_MANUALHOOK(OnAnimationActivityInterrupted, 58 + offset);
	RECONFIGURE_MANUALHOOK(OnAnimationEvent, 59 + offset);
	RECONFIGURE_MANUALHOOK(OnIgnite, 60 + offset);
	RECONFIGURE_MANUALHOOK(OnInjured, 61 + offset);
	RECONFIGURE_MANUALHOOK(OnKilled, 62 + offset);
	RECONFIGURE_MANUALHOOK(OnOtherKilled, 63 + offset);
	RECONFIGURE_MANUALHOOK(OnSight, 64 + offset);
	RECONFIGURE_MANUALHOOK(OnLostSight, 65 + offset);
	RECONFIGURE_MANUALHOOK(OnThreatChanged, 66 + offset);
	RECONFIGURE_MANUALHOOK(OnSound, 67 + offset);
	RECONFIGURE_MANUALHOOK(OnSpokeConcept, 68 + offset);
	RECONFIGURE_MANUALHOOK(OnNavAreaChanged, 69 + offset);
	RECONFIGURE_MANUALHOOK(OnModelChanged, 70 + offset);
	RECONFIGURE_MANUALHOOK(OnPickUp, 71 + offset);
	RECONFIGURE_MANUALHOOK(OnDrop, 72 + offset);
	RECONFIGURE_MANUALHOOK(OnShoved, 73 + offset);
	RECONFIGURE_MANUALHOOK(OnBlinded, 74 + offset);

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		RECONFIGURE_MANUALHOOK(OnCommandApproachVector, 80);
		#ifndef __linux__
			RECONFIGURE_MANUALHOOK(OnCommandAttack, 77);
			RECONFIGURE_MANUALHOOK(OnCommandRetreat, 81);
			RECONFIGURE_MANUALHOOK(OnCommandPause, 82);
			RECONFIGURE_MANUALHOOK(OnCommandResume, 83);
			RECONFIGURE_MANUALHOOK(OnEnteredSpit, 75);
			RECONFIGURE_MANUALHOOK(OnHitByVomitJar, 76);
			RECONFIGURE_MANUALHOOK(OnCommandAssault, 78);
			RECONFIGURE_MANUALHOOK(OnCommandString, 84);
			RECONFIGURE_MANUALHOOK(IsAbleToBlockMovementOf, 85);

			RECONFIGURE_MANUALHOOK(OnCommandApproachEntity, 79);
		#else
			RECONFIGURE_MANUALHOOK(OnCommandAttack, 78);
			RECONFIGURE_MANUALHOOK(OnCommandRetreat, 82);
			RECONFIGURE_MANUALHOOK(OnCommandPause, 83);
			RECONFIGURE_MANUALHOOK(OnCommandResume, 84);
			RECONFIGURE_MANUALHOOK(OnEnteredSpit, 76);
			RECONFIGURE_MANUALHOOK(OnHitByVomitJar, 77);
			RECONFIGURE_MANUALHOOK(OnCommandAssault, 79);
			RECONFIGURE_MANUALHOOK(OnCommandString, 85);
			RECONFIGURE_MANUALHOOK(IsAbleToBlockMovementOf, 86);

			RECONFIGURE_MANUALHOOK(OnCommandApproachEntity, 81);
		#endif
	#else
		RECONFIGURE_MANUALHOOK(OnCommandApproachVector, 72);
		#ifndef __linux__
			RECONFIGURE_MANUALHOOK(OnCommandAttack, 70);
			RECONFIGURE_MANUALHOOK(OnCommandRetreat, 73);
			RECONFIGURE_MANUALHOOK(OnCommandPause, 74);
			RECONFIGURE_MANUALHOOK(OnCommandResume, 75);
			RECONFIGURE_MANUALHOOK(IsAbleToBlockMovementOf, 76);
			RECONFIGURE_MANUALHOOK(OnCommandApproachEntity, 71);
		#else
			RECONFIGURE_MANUALHOOK(OnCommandAttack, 71);
			RECONFIGURE_MANUALHOOK(OnCommandRetreat, 74);
			RECONFIGURE_MANUALHOOK(OnCommandPause, 75);
			RECONFIGURE_MANUALHOOK(OnCommandResume, 76);
			RECONFIGURE_MANUALHOOK(IsAbleToBlockMovementOf, 77);
			RECONFIGURE_MANUALHOOK(OnCommandApproachEntity, 73);
		#endif
	#endif
}

std::map<std::string, size_t>& GetOffsetsInfo()
{
	static std::map<std::string, size_t> map;
	return map;
}
