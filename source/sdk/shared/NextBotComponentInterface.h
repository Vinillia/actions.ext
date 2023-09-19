// NextBotComponentInterface.h
// Interface for all components
// Author: Michael Booth, May 2006
// Copyright (c) 2006 Turtle Rock Studios, Inc. - All Rights Reserved

#ifndef _NEXT_BOT_COMPONENT_INTERFACE_H_
#define _NEXT_BOT_COMPONENT_INTERFACE_H_

#include "NextBotEventResponderInterface.h"

class Path;
class CGameTrace;
class CTakeDamageInfo;
class INextBot;

//--------------------------------------------------------------------------------------------------------------------------
/**
 * Next Bot component interface
 */
class INextBotComponent : public INextBotEventResponder
{
	friend class INextBot;

public:
	INextBotComponent(INextBot* bot);

	virtual ~INextBotComponent() = default;

	virtual void Reset(void);
	virtual void Update(void) {};
	virtual void Upkeep(void) {};

	inline float GetUpdateInterval();

	virtual INextBot *GetBot( void ) const  { return m_bot; }
	
public:
	float m_lastUpdateTime;
	float m_curInterval;
	
	INextBot *m_bot;
	INextBotComponent *m_nextComponent;									// simple linked list of components in the bot
};

inline float INextBotComponent::GetUpdateInterval() 
{ 
	return m_curInterval; 
}

#endif // _NEXT_BOT_COMPONENT_INTERFACE_H_
