#pragma once

class NextBotIntention;

class INextBot : public INextBotEventResponder
{
public:
	virtual ~INextBot() = 0;
	virtual void Reset(void) = 0;
	virtual void Update(void) = 0;
	virtual void Upkeep(void) = 0;
	virtual bool IsRemovedOnReset(void) const = 0;
	virtual void nullfunction() = 0;
	virtual CBaseEntity* GetNextBotCombatCharacter(void) const = 0;
	virtual CBaseEntity* MySurvivorBotPointer(void) const = 0;
	virtual void* GetLocomotionInterface(void) const = 0;
	virtual void* GetBodyInterface(void) const = 0;
	virtual NextBotIntention* GetIntentionInterface(void) const = 0;

public:
	friend class INextBotComponent;
	INextBotComponent *m_componentList;

	const void* m_currentPath;

	int m_id;
	bool m_bFlaggedForUpdate;
	int m_tickLastUpdate;

	unsigned int m_debugType;
	mutable int m_debugDisplayLine;

	Vector m_immobileAnchor;
	void* m_immobileCheckTimer;
	void* m_immobileTimer;

	mutable void* m_baseLocomotion;
	mutable void* m_baseBody;
	mutable void* m_baseIntention;
	mutable void* m_baseVision;
};