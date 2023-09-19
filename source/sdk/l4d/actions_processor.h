#pragma once

#ifndef _INCLUDE_ACTIONS_PROCESSOR
#define _INCLUDE_ACTIONS_PROCESSOR

#include "actions_processor_shared.h"

class ActionProcessor : public ActionProcessorShared
{
public:
	virtual EventDesiredResult< CBaseEntity > OnLeaveGround(CBaseEntity* me, CBaseEntity* ground)  override;
	virtual EventDesiredResult< CBaseEntity > OnLandOnGround(CBaseEntity* me, CBaseEntity* ground)  override;
	virtual EventDesiredResult< CBaseEntity > OnContact(CBaseEntity* me, CBaseEntity* other, CGameTrace* result = NULL)  override;
	virtual EventDesiredResult< CBaseEntity > OnMoveToSuccess(CBaseEntity* me, const Path* path)  override;
	virtual EventDesiredResult< CBaseEntity > OnMoveToFailure(CBaseEntity* me, const Path* path, MoveToFailureType reason)  override;
	virtual EventDesiredResult< CBaseEntity > OnStuck(CBaseEntity* me)  override;
	virtual EventDesiredResult< CBaseEntity > OnUnStuck(CBaseEntity* me)  override;
	virtual EventDesiredResult< CBaseEntity > OnPostureChanged(CBaseEntity* me)  override;
	virtual EventDesiredResult< CBaseEntity > OnAnimationActivityComplete(CBaseEntity* me, int activity)  override;
	virtual EventDesiredResult< CBaseEntity > OnAnimationActivityInterrupted(CBaseEntity* me, int activity)  override;
	virtual EventDesiredResult< CBaseEntity > OnAnimationEvent(CBaseEntity* me, animevent_t* event)  override;
	virtual EventDesiredResult< CBaseEntity > OnIgnite(CBaseEntity* me)  override;
	virtual EventDesiredResult< CBaseEntity > OnInjured(CBaseEntity* me, const CTakeDamageInfo& info)  override;
	virtual EventDesiredResult< CBaseEntity > OnKilled(CBaseEntity* me, const CTakeDamageInfo& info)  override;
	virtual EventDesiredResult< CBaseEntity > OnOtherKilled(CBaseEntity* me, CBaseCombatCharacter* victim, const CTakeDamageInfo& info)  override;
	virtual EventDesiredResult< CBaseEntity > OnSight(CBaseEntity* me, CBaseEntity* subject)  override;
	virtual EventDesiredResult< CBaseEntity > OnLostSight(CBaseEntity* me, CBaseEntity* subject)  override;
	virtual EventDesiredResult< CBaseEntity > OnThreatChanged(CBaseEntity* me, CBaseEntity* subject)  override;
	virtual EventDesiredResult< CBaseEntity > OnSound(CBaseEntity* me, CBaseEntity* source, const Vector& pos, KeyValues* keys)  override;
	virtual EventDesiredResult< CBaseEntity > OnSpokeConcept(CBaseEntity* me, CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response, void* unknown)  override;
	virtual EventDesiredResult< CBaseEntity > OnNavAreaChanged(CBaseEntity* me, CNavArea* newArea, CNavArea* oldArea)  override;
	virtual EventDesiredResult< CBaseEntity > OnModelChanged(CBaseEntity* me)  override;
	virtual EventDesiredResult< CBaseEntity > OnPickUp(CBaseEntity* me, CBaseEntity* item, CBaseCombatCharacter* giver)  override;
	virtual EventDesiredResult< CBaseEntity > OnDrop(CBaseEntity* me, CBaseEntity* item)  override;
	virtual EventDesiredResult< CBaseEntity > OnShoved(CBaseEntity* me, CBaseEntity* pusher)  override;
	virtual EventDesiredResult< CBaseEntity > OnBlinded(CBaseEntity* me, CBaseEntity* blinder)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandAttack(CBaseEntity* me, CBaseEntity* victim)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandApproach(CBaseEntity* me, const Vector& pos, float range)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandApproach(CBaseEntity* me, CBaseEntity* goal)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandRetreat(CBaseEntity* me, CBaseEntity* threat, float range)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandPause(CBaseEntity* me, float duration)  override;
	virtual EventDesiredResult< CBaseEntity > OnCommandResume(CBaseEntity* me)  override;
	virtual bool IsAbleToBlockMovementOf(const INextBot* botInMotion) const override;

public:
	virtual QueryResultType			ShouldPickUp(const INextBot* me, CBaseEntity* item) const override;
	virtual QueryResultType			ShouldHurry(const INextBot* me) const override;
	virtual QueryResultType			IsHindrance(const INextBot* me, CBaseEntity* blocker) const override;
	virtual Vector					SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const override;
	virtual QueryResultType			IsPositionAllowed(const INextBot* me, const Vector& pos) const override;
	virtual PathFollower* QueryCurrentPath(const INextBot* me) const override;
	virtual const CKnownEntity* SelectMoreDangerousThreat(const INextBot* me, const CBaseCombatCharacter* subject, const CKnownEntity* threat1, const CKnownEntity* threat2) const override;
};

#endif // !_INCLUDE_ACTIONS_PROCESSOR