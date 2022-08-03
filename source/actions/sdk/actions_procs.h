#ifndef _INCLUDE_ACTIONS_PROCS_H
#define _INCLUDE_ACTIONS_PROCS_H

class ActionProcessor
{
    inline static std::vector<std::string> hookedNames;
public:
	ActionProcessor(CBaseEntity* entity, Action<void>* action);
	ActionProcessor(Action<void>* action);

	~ActionProcessor() = default;

    static const bool ConfigureHooks();
    const bool ShouldStartProcessors();
    void StartProcessors();

public:
	Action<void>* m_action;

protected:
	#ifndef __linux__
	DEFINE_PROCESSOR(0, dctor, void, bool);
	#else
	DEFINE_PROCESSOR(0, dctor, void);
	#endif

	DEFINE_PROCESSOR(1, start, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(12, update, ActionResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(13, end, void, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(14, suspend, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(15, resume, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(16, initialAction, Action<void>*, CBaseEntity*);
	DEFINE_PROCESSOR(17, leaveGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(18, landGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(19, contact, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CGameTrace*);
	DEFINE_PROCESSOR(10, movetoSuccess, EventDesiredResult<void>, CBaseEntity*, const Path*);
	DEFINE_PROCESSOR(20, movetoFailure, EventDesiredResult<void>, CBaseEntity*, const Path*, MoveToFailureType);
	DEFINE_PROCESSOR(21, stuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(22, unstuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(23, postureChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(24, animationActivityComplete, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(25, animationActivityInterrupted, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(26, animationEvent, EventDesiredResult<void>, CBaseEntity*, animevent_t*);
	DEFINE_PROCESSOR(27, ignite, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(28, injured, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(29, killed, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(30, otherKilled, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(31, sight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(32, lostSight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(33, threatChanged, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(34, sound, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const Vector&, KeyValues*);
	DEFINE_PROCESSOR(35, spokeConcept, EventDesiredResult<void>, CBaseEntity*, CBaseCombatCharacter*, AIConcept_t, AI_Response*, void*);
	DEFINE_PROCESSOR(36, navareaChanged, EventDesiredResult<void>, CBaseEntity*, CNavArea*, CNavArea*);
	DEFINE_PROCESSOR(37, modelChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(38, pickup, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(39, drop, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(40, shoved, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(41, blinded, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(42, commandAttack, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(43, commandApproachVector, EventDesiredResult<void>, CBaseEntity*, const Vector&, float);
	DEFINE_PROCESSOR(44, commandApproachEntity, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(45, commandRetreat, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, float);
	DEFINE_PROCESSOR(46, commandPause, EventDesiredResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(47, commandResume, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(48, abletoBlock, bool, const INextBot*);
};

#endif _INCLUDE_ACTIONS_PROCS_H