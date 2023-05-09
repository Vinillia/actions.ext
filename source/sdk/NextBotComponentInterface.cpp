#include "NextBotComponentInterface.h"
#include "NextBotInterface.h"
#include "edict.h"

extern CGlobalVars* gpGlobals;

INextBotComponent::INextBotComponent(INextBot* bot)
{
	m_curInterval = gpGlobals->interval_per_tick;
	m_lastUpdateTime = 0;
	m_bot = bot;

	// register this component with the bot
	bot->RegisterComponent(this);
}

void INextBotComponent::Reset()
{
	m_lastUpdateTime = 0; m_curInterval = gpGlobals->curtime;
}
