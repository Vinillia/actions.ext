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
 * Various processes can invoke a "reply" (ie: callback) via instances of this interface
 */
class INextBotReply
{
public:
	virtual void OnSuccess( INextBot *bot )  { }						// invoked when process completed successfully

	enum FailureReason
	{
		DENIED,
		INTERRUPTED,
		FAILED
	};
	virtual void OnFail( INextBot *bot, FailureReason reason ) { }		// invoked when process failed
};


//--------------------------------------------------------------------------------------------------------------------------
/**
 * Next Bot component interface
 */
class INextBotComponent : public INextBotEventResponder
{
public:
	INextBotComponent(INextBot* bot);

	virtual ~INextBotComponent() { }

	virtual void Reset(void);
	virtual void Update(void) {};
	virtual void Upkeep(void) {};

	inline float GetUpdateInterval();

	virtual INextBot *GetBot( void ) const  { return m_bot; }
	
// private:
	float m_lastUpdateTime;
	float m_curInterval;

	friend class INextBot;
	
	INextBot *m_bot;
	INextBotComponent *m_nextComponent;									// simple linked list of components in the bot
};

inline float INextBotComponent::GetUpdateInterval() 
{ 
	return m_curInterval; 
}

#endif // _NEXT_BOT_COMPONENT_INTERFACE_H_
