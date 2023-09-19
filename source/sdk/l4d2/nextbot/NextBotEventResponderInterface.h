// NextBotEventResponderInterface.h
// Interface for propagating and responding to events
// Author: Michael Booth, May 2006
// Copyright (c) 2006 Turtle Rock Studios, Inc. - All Rights Reserved

#ifndef _NEXT_BOT_EVENT_RESPONDER_INTERFACE_H_
#define _NEXT_BOT_EVENT_RESPONDER_INTERFACE_H_

class Path;
class CTakeDamageInfo;
class CBaseEntity;
class CDOTABaseAbility;
class CGameTrace;
class CBaseCombatCharacter;
class CNavArea;
class CBaseCombatWeapon;
class KeyValues;
class Vector;

typedef int AI_Response;
typedef int AIConcept_t;

struct CSoundParameters;
struct animevent_t;


//--------------------------------------------------------------------------------------------------------------------------
enum MoveToFailureType
{
	FAIL_NO_PATH_EXISTS,
	FAIL_STUCK,
	FAIL_FELL_OFF,
};

//--------------------------------------------------------------------------------------------------------------------------
/**
 * Events propagated to/between components.
 * To add an event, add its signature here and implement its propagation
 * to derived classes via FirstContainedResponder() and NextContainedResponder().
 * NOTE: Also add a translator to the Action class in NextBotBehavior.h.
 */
class INextBotEventResponder
{
public:
	virtual ~INextBotEventResponder() { }
	virtual INextBotEventResponder* FirstContainedResponder(void) const { return nullptr; }
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const { return nullptr; }

	virtual const char* GetDebugString() const { return ""; }

	virtual void OnLeaveGround(CBaseEntity* ground);
	virtual void OnLandOnGround(CBaseEntity* ground);
	virtual void OnContact(CBaseEntity* other, CGameTrace* result = 0);	
	virtual void OnMoveToSuccess(const Path* path);	
	virtual void OnMoveToFailure(const Path* path, MoveToFailureType reason);
	virtual void OnStuck(void);									
	virtual void OnUnStuck(void);								
	virtual void OnPostureChanged(void);						
	virtual void OnAnimationActivityComplete(int activity);		
	virtual void OnAnimationActivityInterrupted(int activity);	
	virtual void OnAnimationEvent(animevent_t* event);			
	virtual void OnIgnite(void);							
	virtual void OnInjured(const CTakeDamageInfo& info);	
	virtual void OnKilled(const CTakeDamageInfo& info);		
	virtual void OnOtherKilled(CBaseCombatCharacter* victim, const CTakeDamageInfo& info);	
	virtual void OnSight(CBaseEntity* subject);			
	virtual void OnLostSight(CBaseEntity* subject);		
	virtual void OnThreatChanged(CBaseEntity* subject);
	virtual void OnSound(CBaseEntity* source, const Vector& pos, KeyValues* keys);				
	virtual void OnSpokeConcept(CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response, void* last);
	virtual void OnNavAreaChanged(CNavArea* newArea, CNavArea* oldArea);	
	virtual void OnModelChanged(void);		
	virtual void OnPickUp(CBaseEntity* item, CBaseCombatCharacter* giver);	
	virtual void OnDrop(CBaseEntity* item);									
	virtual void OnShoved(CBaseEntity* pusher);									
	virtual void OnBlinded(CBaseEntity* blinder);		
	virtual void OnEnteredSpit();									
	virtual void OnHitByVomitJar(CBaseEntity* shover);	
	virtual void OnCommandAttack(CBaseEntity* victim);
	virtual void OnCommandAssault(void);	
	virtual void OnCommandApproach(const Vector& pos, float range = 0.0f);	
	virtual void OnCommandApproach(CBaseEntity* goal);
	virtual void OnCommandRetreat(CBaseEntity* threat, float range = 0.0f);	
	virtual void OnCommandPause(float duration = 0.0f);
	virtual void OnCommandResume(void);
	virtual void OnCommandString(const char* command);	
};

inline void INextBotEventResponder::OnCommandAssault()
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandAssault();
	}
}

inline void INextBotEventResponder::OnEnteredSpit()
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnEnteredSpit();
	}
}

inline void INextBotEventResponder::OnHitByVomitJar(CBaseEntity* shover)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnHitByVomitJar(shover);
	}
}

inline void INextBotEventResponder::OnThreatChanged(CBaseEntity* subject)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnThreatChanged(subject);
	}
}

inline void INextBotEventResponder::OnLeaveGround(CBaseEntity* ground)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnLeaveGround(ground);
	}
}

inline void INextBotEventResponder::OnLandOnGround(CBaseEntity* ground)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnLandOnGround(ground);
	}
}

inline void INextBotEventResponder::OnContact(CBaseEntity* other, CGameTrace* result)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnContact(other, result);
	}
}

inline void INextBotEventResponder::OnMoveToSuccess(const Path* path)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnMoveToSuccess(path);
	}
}

inline void INextBotEventResponder::OnMoveToFailure(const Path* path, MoveToFailureType reason)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnMoveToFailure(path, reason);
	}
}

inline void INextBotEventResponder::OnStuck(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnStuck();
	}
}

inline void INextBotEventResponder::OnUnStuck(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnUnStuck();
	}
}

inline void INextBotEventResponder::OnPostureChanged(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnPostureChanged();
	}
}

inline void INextBotEventResponder::OnAnimationActivityComplete(int activity)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnAnimationActivityComplete(activity);
	}
}

inline void INextBotEventResponder::OnAnimationActivityInterrupted(int activity)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnAnimationActivityInterrupted(activity);
	}
}

inline void INextBotEventResponder::OnAnimationEvent(animevent_t* event)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnAnimationEvent(event);
	}
}

inline void INextBotEventResponder::OnIgnite(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnIgnite();
	}
}

inline void INextBotEventResponder::OnInjured(const CTakeDamageInfo& info)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnInjured(info);
	}
}

inline void INextBotEventResponder::OnKilled(const CTakeDamageInfo& info)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnKilled(info);
	}
}

inline void INextBotEventResponder::OnOtherKilled(CBaseCombatCharacter* victim, const CTakeDamageInfo& info)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnOtherKilled(victim, info);
	}
}

inline void INextBotEventResponder::OnSight(CBaseEntity* subject)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnSight(subject);
	}
}

inline void INextBotEventResponder::OnLostSight(CBaseEntity* subject)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnLostSight(subject);
	}
}

inline void INextBotEventResponder::OnSound(CBaseEntity* source, const Vector& pos, KeyValues* keys)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnSound(source, pos, keys);
	}
}

inline void INextBotEventResponder::OnSpokeConcept(CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response, void* last)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnSpokeConcept(who, aiconcept, response, last);
	}
}

inline void INextBotEventResponder::OnNavAreaChanged(CNavArea* newArea, CNavArea* oldArea)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnNavAreaChanged(newArea, oldArea);
	}
}

inline void INextBotEventResponder::OnModelChanged(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnModelChanged();
	}
}

inline void INextBotEventResponder::OnPickUp(CBaseEntity* item, CBaseCombatCharacter* giver)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnPickUp(item, giver);
	}
}

inline void INextBotEventResponder::OnDrop(CBaseEntity* item)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnDrop(item);
	}
}

inline void INextBotEventResponder::OnShoved(CBaseEntity* pusher)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnShoved(pusher);
	}
}

inline void INextBotEventResponder::OnBlinded(CBaseEntity* blinder)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnBlinded(blinder);
	}
}

inline void INextBotEventResponder::OnCommandAttack(CBaseEntity* victim)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandAttack(victim);
	}
}

inline void INextBotEventResponder::OnCommandApproach(const Vector& pos, float range)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandApproach(pos, range);
	}
}

inline void INextBotEventResponder::OnCommandApproach(CBaseEntity* goal)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandApproach(goal);
	}
}

inline void INextBotEventResponder::OnCommandRetreat(CBaseEntity* threat, float range)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandRetreat(threat, range);
	}
}

inline void INextBotEventResponder::OnCommandPause(float duration)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandPause(duration);
	}
}

inline void INextBotEventResponder::OnCommandResume(void)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandResume();
	}
}

inline void INextBotEventResponder::OnCommandString(const char* command)
{
	for (INextBotEventResponder* sub = FirstContainedResponder(); sub; sub = NextContainedResponder(sub))
	{
		sub->OnCommandString(command);
	}
}

#endif // _NEXT_BOT_EVENT_RESPONDER_INTERFACE_H_