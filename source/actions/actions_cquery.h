#pragma once

#ifndef _INCLUDE_ACTIONS_CQUERY_H
#define _INCLUDE_ACTIONS_CQUERY_H

extern const bool ConfigureContextualHooks();

class ActionContextualProcessor : public ActionProcessor
{
public:
    ActionContextualProcessor(CBaseEntity* entity, Action<void>* action);
	ActionContextualProcessor(Action<void>* action);

    ~ActionContextualProcessor() = default;

public:
	DEFINE_PROCESSOR(100, shouldPickUp, QueryResultType, INextBot*, CBaseEntity*);
	DEFINE_PROCESSOR(101, shouldHurry, QueryResultType, INextBot*);
	DEFINE_PROCESSOR(102, isHindrance, QueryResultType, INextBot*, CBaseEntity*);
	DEFINE_PROCESSOR(103, selectTargetPoint, Vector, INextBot*, CBaseCombatCharacter*);
	DEFINE_PROCESSOR(104, isPositionAllowed, QueryResultType, INextBot*, Vector&);
	DEFINE_PROCESSOR(105, queryCurrentPath, PathFollower*, INextBot*);
	DEFINE_PROCESSOR(106, selectMoreDangerousThreat, CKnownEntity*, INextBot*, CBaseCombatCharacter*, CKnownEntity*, CKnownEntity*);
};

#endif // _INCLUDE_ACTIONS_CQUERY_H