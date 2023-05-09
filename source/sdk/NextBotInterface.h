#ifndef _INCLUDE_NEXTBOT_INTERFACE_H
#define _INCLUDE_NEXTBOT_INTERFACE_H

#include "NextBotEventResponderInterface.h"
#include "NextBotDebug.h"

#include "Color.h"
#include "vector.h"
#include "utlvector.h"

class NextBotIntention;
class CBaseEntity;
class INextBotComponent;

class INextBot : public INextBotEventResponder
{
public:
	INextBot(void);
	virtual ~INextBot();

	int GetBotId() const;

	virtual void Reset(void) = 0;										// (EXTEND) reset to initial state
	virtual void Update(void) = 0;									// (EXTEND) update internal state
	virtual void Upkeep(void) = 0;									// (EXTEND) lightweight update guaranteed to occur every server tick

	void FlagForUpdate(bool b = true);
	bool IsFlaggedForUpdate();
	int GetTickLastUpdate() const;
	void SetTickLastUpdate(int);

	virtual bool IsRemovedOnReset(void) const { return true; }	// remove this bot when the NextBot manager calls Reset

	virtual CBaseCombatCharacter* GetEntity(void) const = 0;
	virtual class NextBotCombatCharacter* GetNextBotCombatCharacter(void) const { return nullptr; }

	virtual class SurvivorBot* MySurvivorBotPointer() const { return nullptr; }

	// interfaces are never NULL - return base no-op interfaces at a minimum
	virtual class ILocomotion* GetLocomotionInterface(void) const = 0;
	virtual class IBody* GetBodyInterface(void) const = 0;
	virtual class IIntention* GetIntentionInterface(void) const = 0;
	virtual class IVision* GetVisionInterface(void) const = 0;

	/**
	 * Attempt to change the bot's position. Return true if successful.
	 */
	virtual bool SetPosition(const Vector& pos) = 0;
	virtual const Vector& GetPosition(void) const = 0;				// get the global position of the bot

	/**
	 * Friend/enemy/neutral queries
	 */
	virtual bool IsEnemy(const CBaseEntity* them) const = 0;			// return true if given entity is our enemy
	virtual bool IsFriend(const CBaseEntity* them) const = 0;			// return true if given entity is our friend
	virtual bool IsSelf(const CBaseEntity* them) const = 0;			// return true if 'them' is actually me

	virtual bool IsAllowedToClimb() const = 0;
	virtual bool ReactToSurvivorVisibility() const = 0;
	virtual bool ReactToSurvivorNoise() const = 0;
	virtual bool ReactToSurvivorContact() const = 0;

	/**
	 * Can we climb onto this entity?
	 */
	virtual bool IsAbleToClimbOnto(const CBaseEntity* object) const = 0;

	/**
	 * Can we break this entity?
	 */
	virtual bool IsAbleToBreak(const CBaseEntity* object) const = 0;

	/**
	 * Sometimes we want to pass through other NextBots. OnContact() will always
	 * be invoked, but collision resolution can be skipped if this
	 * method returns false.
	 */
	virtual bool IsAbleToBlockMovementOf(const INextBot* botInMotion) const { return true; }

	/**
	 * Should we ever care about noticing physical contact with this entity?
	 */
	virtual bool ShouldTouch(const CBaseEntity* object) const { return true; }

	/**
	 * This immobile system is used to track the global state of "am I actually moving or not".
	 * The OnStuck() event is only emitted when following a path, and paths can be recomputed, etc.
	 */
	virtual bool IsImmobile(void) const = 0;					// return true if we haven't moved in awhile
	virtual float GetImmobileDuration(void) const = 0;		// how long have we been immobile
	virtual void ClearImmobileStatus(void) = 0;
	virtual float GetImmobileSpeedThreshold(void) const = 0;	// return units/second below which this actor is considered "immobile"


	// between distance utility methods
	virtual bool IsRangeLessThan(CBaseEntity* subject, float range) const = 0;
	virtual bool IsRangeLessThan(const Vector& pos, float range) const = 0;
	virtual bool IsRangeGreaterThan(CBaseEntity* subject, float range) const = 0;
	virtual bool IsRangeGreaterThan(const Vector& pos, float range) const = 0;
	virtual float GetRangeTo(CBaseEntity* subject) const = 0;
	virtual float GetRangeTo(const Vector& pos) const = 0;
	virtual float GetRangeSquaredTo(CBaseEntity* subject) const = 0;
	virtual float GetRangeSquaredTo(const Vector& pos) const = 0;

	virtual float Get2DRangeTo(Vector const&) const = 0;
	virtual float Get2DRangeTo(CBaseEntity*) const = 0;

	// event propagation
	virtual INextBotEventResponder* FirstContainedResponder(void) const override;
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const override;

	virtual const char* GetDebugIdentifier(void) const = 0;		// return the name of this bot for debugging purposes
	virtual bool IsDebugFilterMatch(const char* name) const = 0;	// return true if we match the given debug symbol
	virtual void DisplayDebugText(const char* text) const = 0;	// show a line of text on the bot in the world

	enum {
		MAX_NEXTBOT_DEBUG_HISTORY = 100,
		MAX_NEXTBOT_DEBUG_LINE_LENGTH = 256,
	};
	struct NextBotDebugLineType
	{
		NextBotDebugType debugType;
		char data[MAX_NEXTBOT_DEBUG_LINE_LENGTH];
	};

	/*const char* GetDebugIdentifier(void) const;*/
	bool IsDebugging(unsigned int type) const;
	void DebugConColorMsg(enum NextBotDebugType debugType, const Color& color, const char* fmt, ...);
	void ResetDebugHistory(void);

public:
	friend class INextBotComponent;
	void RegisterComponent(INextBotComponent* comp);		// components call this to register themselves with the bot that contains them
	INextBotComponent* m_componentList;						// the first component

	int m_id;
	bool m_bFlaggedForUpdate;
	int m_tickLastUpdate;

	unsigned int m_debugType;
	mutable int m_debugDisplayLine;

	Vector m_immobileAnchor;
	void* m_immobileCheckTimer;
	void* m_immobileTimer;
	
	mutable class ILocomotion*	m_baseLocomotion;
	mutable class IBody*		m_baseBody;
	mutable class IIntention*	m_baseIntention;
	mutable class IVision*		m_baseVision;

	CUtlVector< NextBotDebugLineType* > m_debugHistory;
};

inline int INextBot::GetBotId() const
{
	return m_id;
}

inline void INextBot::FlagForUpdate(bool b)
{
	m_bFlaggedForUpdate = b;
}

inline bool INextBot::IsFlaggedForUpdate()
{
	return m_bFlaggedForUpdate;
}

inline int INextBot::GetTickLastUpdate() const
{
	return m_tickLastUpdate;
}

inline void INextBot::SetTickLastUpdate(int tick)
{
	m_tickLastUpdate = tick;
}

inline float INextBot::GetImmobileSpeedThreshold(void) const
{
	return 30.0f;
}

#endif // !_INCLUDE_NEXTBOT_INTERFACE_H
