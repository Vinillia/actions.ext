// NextBotContextualQueryInterface.h
// Queries within the context of the bot's current behavior state
// Author: Michael Booth, June 2007
// Copyright (c) 2007 Turtle Rock Studios, Inc. - All Rights Reserved

#ifndef _NEXT_BOT_CONTEXTUAL_QUERY_H_
#define _NEXT_BOT_CONTEXTUAL_QUERY_H_

#include <vector.h>

class INextBot;
class CBaseEntity;
class CBaseCombatCharacter;
class Path;
class CKnownEntity;
class PathFollower;

/**
 * Since behaviors can have several concurrent actions active, we ask
 * the topmost child action first, and if it defers, its parent, and so
 * on, until we get a definitive answer.
 */
enum QueryResultType
{
	ANSWER_NO,
	ANSWER_YES,
	ANSWER_UNDEFINED
};

// Can pass this into IContextualQuery::IsHindrance to see if any hindrance is ever possible
#define IS_ANY_HINDRANCE_POSSIBLE	( (CBaseEntity*)0xFFFFFFFF )


//----------------------------------------------------------------------------------------------------------------
/**
 * The interface for queries that are dependent on the bot's current behavior state
 */
class IContextualQuery
{
public:
	virtual ~IContextualQuery() { }

	virtual QueryResultType			ShouldPickUp(const INextBot* me, CBaseEntity* item) const;								
	virtual QueryResultType			ShouldHurry(const INextBot* me) const;													
	virtual QueryResultType			ShouldRetreat(const INextBot* me) const;											
	virtual QueryResultType			ShouldAttack(const INextBot* me, const CKnownEntity* threat) const;											
	virtual QueryResultType			IsHindrance(const INextBot* me, CBaseEntity* blocker) const;							
	virtual Vector					SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const;		
	virtual QueryResultType IsPositionAllowed(const INextBot* me, const Vector& pos) const;
	virtual const CKnownEntity* SelectMoreDangerousThreat(const INextBot* me,
		const CBaseCombatCharacter* subject,
		const CKnownEntity* threat1,
		const CKnownEntity* threat2) const;
};

inline QueryResultType IContextualQuery::ShouldPickUp(const INextBot* me, CBaseEntity* item) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldHurry(const INextBot* me) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldRetreat(const INextBot* me) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldAttack(const INextBot* me, const CKnownEntity* threat) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::IsHindrance(const INextBot* me, CBaseEntity* blocker) const
{
	return ANSWER_UNDEFINED;
}

inline Vector IContextualQuery::SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const
{
	return Vector(0.0f, 0.0f, 0.0f);
}

inline QueryResultType IContextualQuery::IsPositionAllowed(const INextBot* me, const Vector& pos) const
{
	return ANSWER_UNDEFINED;
}

inline const CKnownEntity* IContextualQuery::SelectMoreDangerousThreat(const INextBot* me, const CBaseCombatCharacter* subject, const CKnownEntity* threat1, const CKnownEntity* threat2) const
{
	return NULL;
}


#endif // _NEXT_BOT_CONTEXTUAL_QUERY_H_