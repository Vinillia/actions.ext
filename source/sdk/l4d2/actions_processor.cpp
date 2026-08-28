#include "actions_processor.h"

ActionProcessor g_ActionProcessor;

//-----------------------------------------------------------------------------
// Action Handlers
//-----------------------------------------------------------------------------
EventDesiredResult<CBaseEntity> ActionProcessor::OnLeaveGround(CBaseEntity* me, CBaseEntity* ground)
{
	return ProcessHandler(gProcessorFunctions->OnLeaveGround, this, &ActionProcessor::OnLeaveGround, me, ground);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnLandOnGround(CBaseEntity* me, CBaseEntity* ground)
{
	return ProcessHandler(gProcessorFunctions->OnLandOnGround, this, &ActionProcessor::OnLandOnGround, me, ground);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnContact(CBaseEntity* me, CBaseEntity* other, CGameTrace* trace)
{
	return ProcessHandler(gProcessorFunctions->OnContact, this, &ActionProcessor::OnContact, me, other, trace);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnMoveToSuccess(CBaseEntity* me, const Path* path)
{
	return ProcessHandler(gProcessorFunctions->OnMoveToSuccess, this, &ActionProcessor::OnMoveToSuccess, me, path);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnMoveToFailure(CBaseEntity* me, const Path* path, MoveToFailureType reason)
{
	return ProcessHandler(gProcessorFunctions->OnMoveToFailure, this, &ActionProcessor::OnMoveToFailure, me, path, reason);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnStuck(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnStuck, this, &ActionProcessor::OnStuck, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnUnStuck(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnUnStuck, this, &ActionProcessor::OnUnStuck, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnPostureChanged(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnPostureChanged, this, &ActionProcessor::OnPostureChanged, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnAnimationActivityComplete(CBaseEntity* me, int activity)
{
	return ProcessHandler(gProcessorFunctions->OnAnimationActivityComplete, this, &ActionProcessor::OnAnimationActivityComplete, me, activity);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnAnimationActivityInterrupted(CBaseEntity* me, int activity)
{
	return ProcessHandler(gProcessorFunctions->OnAnimationActivityInterrupted, this, &ActionProcessor::OnAnimationActivityInterrupted, me, activity);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnAnimationEvent(CBaseEntity* me, animevent_t* event)
{
	return ProcessHandler(gProcessorFunctions->OnAnimationEvent, this, &ActionProcessor::OnAnimationEvent, me, event);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnIgnite(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnIgnite, this, &ActionProcessor::OnIgnite, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnInjured(CBaseEntity* me, const CTakeDamageInfo& info)
{
	return ProcessHandler(gProcessorFunctions->OnInjured, this, &ActionProcessor::OnInjured, me, info);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnKilled(CBaseEntity* me, const CTakeDamageInfo& info)
{
	return ProcessHandler(gProcessorFunctions->OnKilled, this, &ActionProcessor::OnKilled, me, info);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnOtherKilled(CBaseEntity* me, CBaseCombatCharacter* victim, const CTakeDamageInfo& info)
{
	return ProcessHandler(gProcessorFunctions->OnOtherKilled, this, &ActionProcessor::OnOtherKilled, me, victim, info);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnSight(CBaseEntity* me, CBaseEntity* subject)
{
	return ProcessHandler(gProcessorFunctions->OnSight, this, &ActionProcessor::OnSight, me, subject);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnLostSight(CBaseEntity* me, CBaseEntity* subject)
{
	return ProcessHandler(gProcessorFunctions->OnLostSight, this, &ActionProcessor::OnLostSight, me, subject);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnThreatChanged(CBaseEntity* me, CBaseEntity* subject)
{
	return ProcessHandler(gProcessorFunctions->OnThreatChanged, this, &ActionProcessor::OnThreatChanged, me, subject);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnSound(CBaseEntity* me, CBaseEntity* source, const Vector& pos, KeyValues* keys)
{
	return ProcessHandler(gProcessorFunctions->OnSound, this, &ActionProcessor::OnSound, me, source, pos, keys);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnSpokeConcept(CBaseEntity* me, CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response, void* unknown)
{
	return ProcessHandler(gProcessorFunctions->OnSpokeConcept, this, &ActionProcessor::OnSpokeConcept, me, who, aiconcept, response, unknown);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnNavAreaChanged(CBaseEntity* me, CNavArea* newArea, CNavArea* oldArea)
{
	return ProcessHandler(gProcessorFunctions->OnNavAreaChanged, this, &ActionProcessor::OnNavAreaChanged, me, newArea, oldArea);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnModelChanged(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnModelChanged, this, &ActionProcessor::OnModelChanged, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnPickUp(CBaseEntity* me, CBaseEntity* item, CBaseCombatCharacter* giver)
{
	return ProcessHandler(gProcessorFunctions->OnPickUp, this, &ActionProcessor::OnPickUp, me, item, giver);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnDrop(CBaseEntity* me, CBaseEntity* item)
{
	return ProcessHandler(gProcessorFunctions->OnDrop, this, &ActionProcessor::OnDrop, me, item);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnShoved(CBaseEntity* me, CBaseEntity* pusher)
{
	return ProcessHandler(gProcessorFunctions->OnShoved, this, &ActionProcessor::OnShoved, me, pusher);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnBlinded(CBaseEntity* me, CBaseEntity* blinder)
{
	return ProcessHandler(gProcessorFunctions->OnBlinded, this, &ActionProcessor::OnBlinded, me, blinder);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnEnteredSpit(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnEnteredSpit, this, &ActionProcessor::OnEnteredSpit, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnHitByVomitJar(CBaseEntity* me, CBaseEntity* owner)
{
	return ProcessHandler(gProcessorFunctions->OnHitByVomitJar, this, &ActionProcessor::OnHitByVomitJar, me, owner);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandAttack(CBaseEntity* me, CBaseEntity* victim)
{
	return ProcessHandler(gProcessorFunctions->OnCommandAttack, this, &ActionProcessor::OnCommandAttack, me, victim);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandAssault(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnCommandAssault, this, &ActionProcessor::OnCommandAssault, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandApproach(CBaseEntity* me, const Vector& pos, float range)
{
	using OnCommandApproachByVector_t = EventDesiredResult<CBaseEntity>(ActionProcessor::*)(CBaseEntity*, const Vector&, float);
	return ProcessHandler(gProcessorFunctions->OnCommandApproachByVector, this, (OnCommandApproachByVector_t)&ActionProcessor::OnCommandApproach, me, pos, range);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandApproach(CBaseEntity* me, CBaseEntity* goal)
{
	using OnCommandApproachByEntity_t = EventDesiredResult<CBaseEntity>(ActionProcessor::*)(CBaseEntity*, CBaseEntity*);
	return ProcessHandler(gProcessorFunctions->OnCommandApproachByEntity, this, (OnCommandApproachByEntity_t)&ActionProcessor::OnCommandApproach, me, goal);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandRetreat(CBaseEntity* me, CBaseEntity* threat, float range)
{
	return ProcessHandler(gProcessorFunctions->OnCommandRetreat, this, &ActionProcessor::OnCommandRetreat, me, threat, range);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandPause(CBaseEntity* me, float duration)
{
	return ProcessHandler(gProcessorFunctions->OnCommandPause, this, &ActionProcessor::OnCommandPause, me, duration);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandResume(CBaseEntity* me)
{
	return ProcessHandler(gProcessorFunctions->OnCommandResume, this, &ActionProcessor::OnCommandResume, me);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnCommandString(CBaseEntity* me, const char* command)
{
	return ProcessHandler(gProcessorFunctions->OnCommandString, this, &ActionProcessor::OnCommandString, me, command);
}

bool ActionProcessor::IsAbleToBlockMovementOf(const INextBot* botInMotion) const
{
	return ProcessHandler(gProcessorFunctions->IsAbleToBlockMovementOf, const_cast<ActionProcessor*>(this), &ActionProcessor::IsAbleToBlockMovementOf, botInMotion);
}

QueryResultType ActionProcessor::ShouldPickUp(const INextBot* me, CBaseEntity* item) const
{
	return ProcessHandler(gProcessorFunctions->ShouldPickUp, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldPickUp, me, item);
}

QueryResultType ActionProcessor::ShouldHurry(const INextBot* me) const
{
	return ProcessHandler(gProcessorFunctions->ShouldHurry, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldHurry, me);
}

QueryResultType ActionProcessor::IsHindrance(const INextBot* me, CBaseEntity* blocker) const
{
	return ProcessHandler(gProcessorFunctions->IsHindrance, const_cast<ActionProcessor*>(this), &IContextualQuery::IsHindrance, me, blocker);
}

Vector ActionProcessor::SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const
{
	return ProcessHandler(gProcessorFunctions->SelectTargetPoint, const_cast<ActionProcessor*>(this), &IContextualQuery::SelectTargetPoint, me, subject);
}

QueryResultType ActionProcessor::IsPositionAllowed(const INextBot* me, const Vector& pos) const
{
	return ProcessHandler(gProcessorFunctions->IsPositionAllowed, const_cast<ActionProcessor*>(this), &IContextualQuery::IsPositionAllowed, me, pos);
}

PathFollower* ActionProcessor::QueryCurrentPath(const INextBot* me) const
{
	return ProcessHandler(gProcessorFunctions->QueryCurrentPath, const_cast<ActionProcessor*>(this), &IContextualQuery::QueryCurrentPath, me);
}

const CKnownEntity* ActionProcessor::SelectMoreDangerousThreat(const INextBot* me, const CBaseCombatCharacter* subject, const CKnownEntity* threat1, const CKnownEntity* threat2) const
{
	return ProcessHandler(gProcessorFunctions->SelectMoreDangerousThreat, const_cast<ActionProcessor*>(this), &IContextualQuery::SelectMoreDangerousThreat, me, subject, threat1, threat2);
}