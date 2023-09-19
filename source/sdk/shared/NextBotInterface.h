#ifndef _INCLUDE_NEXTBOT_INTERFACE_H
#define _INCLUDE_NEXTBOT_INTERFACE_H

#include "NextBotEventResponderInterface.h"
#include "NextBotDebug.h"
#include "NextBotComponentInterface.h"

#include "Color.h"
#include "vector.h"
#include "utlvector.h"

class CBaseEntity;
class INextBotComponent;

class INextBot : public INextBotEventResponder
{
public:
	INextBot(void) = default;
	virtual ~INextBot() = 0;

	virtual INextBotEventResponder* FirstContainedResponder(void) const override;
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const override;

public:
	friend class INextBotComponent;

	const char* GetDebugIdentifier(void) const;
	void RegisterComponent(INextBotComponent* comp);		// components call this to register themselves with the bot that contains them

	INextBotComponent* m_componentList;
};

inline void INextBot::RegisterComponent(INextBotComponent* comp)
{
	comp->m_nextComponent = m_componentList;
	m_componentList = comp;
}

inline INextBotEventResponder* INextBot::FirstContainedResponder(void) const
{
	return m_componentList;
}

inline INextBotEventResponder* INextBot::NextContainedResponder(INextBotEventResponder* current) const
{
	return reinterpret_cast<INextBotComponent*>(current)->m_nextComponent;
}



#endif // !_INCLUDE_NEXTBOT_INTERFACE_H
