#include "actions_processor.h"

ActionProcessor g_ActionProcessor;

//-----------------------------------------------------------------------------
// Action Handlers
//-----------------------------------------------------------------------------

EventDesiredResult< CBaseEntity > ActionProcessor::OnLeaveGround(CBaseEntity* me, CBaseEntity* ground)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLeaveGround");
	return ProcessHandler(hash, this, &ActionProcessor::OnLeaveGround, me, ground);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLandOnGround(CBaseEntity* me, CBaseEntity* ground)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLandOnGround");
	return ProcessHandler(hash, this, &ActionProcessor::OnLandOnGround, me, ground);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnContact(CBaseEntity* me, CBaseEntity* other, CGameTrace* trace)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnContact");
	return ProcessHandler(hash, this, &ActionProcessor::OnContact, me, other, trace);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnMoveToSuccess(CBaseEntity* me, const Path* path)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnMoveToSuccess");
	return ProcessHandler(hash, this, &ActionProcessor::OnMoveToSuccess, me, path);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnMoveToFailure(CBaseEntity* me, const Path* path, MoveToFailureType reason)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnMoveToFailure");
	return ProcessHandler(hash, this, &ActionProcessor::OnMoveToFailure, me, path, reason);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnStuck(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnStuck");
	return ProcessHandler(hash, this, &ActionProcessor::OnStuck, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnUnStuck(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnUnStuck");
	return ProcessHandler(hash, this, &ActionProcessor::OnUnStuck, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnPostureChanged(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnPostureChanged");
	return ProcessHandler(hash, this, &ActionProcessor::OnPostureChanged, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationActivityComplete(CBaseEntity* me, int activity)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationActivityComplete");
	return ProcessHandler(hash, this, &ActionProcessor::OnAnimationActivityComplete, me, activity);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationActivityInterrupted(CBaseEntity* me, int activity)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationActivityInterrupted");
	return ProcessHandler(hash, this, &ActionProcessor::OnAnimationActivityInterrupted, me, activity);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationEvent(CBaseEntity* me, animevent_t* event)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationEvent");
	return ProcessHandler(hash, this, &ActionProcessor::OnAnimationEvent, me, event);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnIgnite(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnIgnite");
	return ProcessHandler(hash, this, &ActionProcessor::OnIgnite, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnInjured(CBaseEntity* me, const CTakeDamageInfo& info)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnInjured");
	return ProcessHandler(hash, this, &ActionProcessor::OnInjured, me, info);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnKilled(CBaseEntity* me, const CTakeDamageInfo& info)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnKilled");
	return ProcessHandler(hash, this, &ActionProcessor::OnKilled, me, info);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnOtherKilled(CBaseEntity* me, CBaseCombatCharacter* victim, const CTakeDamageInfo& info)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnOtherKilled");
	return ProcessHandler(hash, this, &ActionProcessor::OnOtherKilled, me, victim, info);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSight(CBaseEntity* me, CBaseEntity* subject)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSight");
	return ProcessHandler(hash, this, &ActionProcessor::OnSight, me, subject);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLostSight(CBaseEntity* me, CBaseEntity* subject)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLostSight");
	return ProcessHandler(hash, this, &ActionProcessor::OnLostSight, me, subject);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSound(CBaseEntity* me, CBaseEntity* source, const Vector& pos, KeyValues* keys)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSound");
	return ProcessHandler(hash, this, &ActionProcessor::OnSound, me, source, pos, keys);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSpokeConcept(CBaseEntity* me, CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSpokeConcept");
	return ProcessHandler(hash, this, &ActionProcessor::OnSpokeConcept, me, who, aiconcept, response);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnWeaponFired(CBaseEntity* me, CBaseCombatCharacter* whoFired, CBaseCombatWeapon* weapon)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnWeaponFired");
	return ProcessHandler(hash, this, &ActionProcessor::OnWeaponFired, me, whoFired, weapon);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnNavAreaChanged(CBaseEntity* me, CNavArea* newArea, CNavArea* oldArea)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnNavAreaChanged");
	return ProcessHandler(hash, this, &ActionProcessor::OnNavAreaChanged, me, newArea, oldArea);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnModelChanged(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnModelChanged");
	return ProcessHandler(hash, this, &ActionProcessor::OnModelChanged, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnPickUp(CBaseEntity* me, CBaseEntity* item, CBaseCombatCharacter* giver)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnPickUp");
	return ProcessHandler(hash, this, &ActionProcessor::OnPickUp, me, item, giver);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnDrop(CBaseEntity* me, CBaseEntity* item)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnDrop");
	return ProcessHandler(hash, this, &ActionProcessor::OnDrop, me, item);
}

EventDesiredResult<CBaseEntity> ActionProcessor::OnActorEmoted(CBaseEntity* me, CBaseCombatCharacter* emoter, int emote)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnActorEmoted");
	return ProcessHandler(hash, this, &ActionProcessor::OnActorEmoted, me, emoter, emote);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnShoved(CBaseEntity* me, CBaseEntity* pusher)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnShoved");
	return ProcessHandler(hash, this, &ActionProcessor::OnShoved, me, pusher);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnBlinded(CBaseEntity* me, CBaseEntity* blinder)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnBlinded");
	return ProcessHandler(hash, this, &ActionProcessor::OnBlinded, me, blinder);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandAttack(CBaseEntity* me, CBaseEntity* victim)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandAttack");
	return ProcessHandler(hash, this, &ActionProcessor::OnCommandAttack, me, victim);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandApproach(CBaseEntity* me, const Vector& pos, float range)
{
	using OnCommandApproach_t = EventDesiredResult<CBaseEntity>(ActionProcessor::*)(CBaseEntity*, const Vector&, float);
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandApproachByVector");
	return ProcessHandler(hash, this, (OnCommandApproach_t)&ActionProcessor::OnCommandApproach, me, pos, range);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandApproach(CBaseEntity* me, CBaseEntity* goal)
{
	using OnCommandApproach_t = EventDesiredResult<CBaseEntity>(ActionProcessor::*)(CBaseEntity*, CBaseEntity*);
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandApproachByEntity");
	return ProcessHandler(hash, this, (OnCommandApproach_t)&ActionProcessor::OnCommandApproach, me, goal);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandRetreat(CBaseEntity* me, CBaseEntity* threat, float range)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandRetreat");
	return ProcessHandler(hash, this, &ActionProcessor::OnCommandRetreat, me, threat, range);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandPause(CBaseEntity* me, float duration)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandPause");
	return ProcessHandler(hash, this, &ActionProcessor::OnCommandPause, me, duration);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandResume(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandResume");
	return ProcessHandler(hash, this, &ActionProcessor::OnCommandResume, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandString(CBaseEntity* me, const char* command)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandString");
	return ProcessHandler(hash, this, &ActionProcessor::OnCommandString, me, command);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnTerritoryContested(CBaseEntity* me, int territoryID)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnTerritoryContested");
	return ProcessHandler(hash, this, &ActionProcessor::OnTerritoryContested, me, territoryID);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnTerritoryCaptured(CBaseEntity* me, int territoryID)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnTerritoryCaptured");
	return ProcessHandler(hash, this, &ActionProcessor::OnTerritoryCaptured, me, territoryID);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnTerritoryLost(CBaseEntity* me, int territoryID)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnTerritoryLost");
	return ProcessHandler(hash, this, &ActionProcessor::OnTerritoryLost, me, territoryID);
}
EventDesiredResult< CBaseEntity > ActionProcessor::OnWin(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnWin");
	return ProcessHandler(hash, this, &ActionProcessor::OnWin, me);
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLose(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLose");
	return ProcessHandler(hash, this, &ActionProcessor::OnLose, me);
}

bool ActionProcessor::IsAbleToBlockMovementOf(const INextBot* botInMotion) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::IsAbleToBlockMovementOf");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &ActionProcessor::IsAbleToBlockMovementOf, botInMotion);
}

QueryResultType ActionProcessor::ShouldPickUp(const INextBot* me, CBaseEntity* item) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldPickUp");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldPickUp, me, item);
}

QueryResultType ActionProcessor::ShouldHurry(const INextBot* me) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldHurry");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldHurry, me);
}

QueryResultType ActionProcessor::ShouldAttack(const INextBot* me, const CKnownEntity* threat) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldAttack");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldAttack, me, threat);
}

QueryResultType ActionProcessor::ShouldRetreat(const INextBot* me) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldRetreat");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::ShouldRetreat, me);
}

QueryResultType ActionProcessor::IsHindrance(const INextBot* me, CBaseEntity* blocker) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::IsHindrance");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::IsHindrance, me, blocker);
}

Vector ActionProcessor::SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::SelectTargetPoint");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::SelectTargetPoint, me, subject);
}

QueryResultType ActionProcessor::IsPositionAllowed(const INextBot* me, const Vector& pos) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::IsPositionAllowed");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::IsPositionAllowed, me, pos);
}

const CKnownEntity* ActionProcessor::SelectMoreDangerousThreat(const INextBot* me, const CBaseCombatCharacter* subject, const CKnownEntity* threat1, const CKnownEntity* threat2) const
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::SelectMoreDangerousThreat");
	return ProcessHandler(hash, const_cast<ActionProcessor*>(this), &IContextualQuery::SelectMoreDangerousThreat, me, subject, threat1, threat2);
}