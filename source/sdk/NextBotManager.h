// NextBotManager.h
// Author: Michael Booth, May 2006
// Copyright (c) 2006 Turtle Rock Studios, Inc. - All Rights Reserved

#ifndef _NEXT_BOT_MANAGER_H_
#define _NEXT_BOT_MANAGER_H_

#include <utlvector.h>
#include <utllinkedlist.h>

class CTerrorPlayer;
class INextBot;

//----------------------------------------------------------------------------------------------------------------
/**
 * The NextBotManager manager 
 */
class NextBotManager
{
	virtual void vptr() = 0;

public:
	int GetNextBotCount( void ) const;				// How many nextbots are alive right now?

	void Select( INextBot *bot );					// mark bot as selected for further operations
	void DeselectAll( void );

	INextBot *GetSelected( void ) const;

	friend class INextBot;

	CUtlLinkedList< INextBot * > m_botList;				// list of all active NextBots

private:
	int m_iUpdateTickrate;
	double m_CurUpdateStartTime;
	double m_SumFrameTime;

	unsigned int m_debugType;	

	struct DebugFilter
	{
		int index;			// entindex
		enum { MAX_DEBUG_NAME_SIZE = 128 };
		char name[ MAX_DEBUG_NAME_SIZE ];
	};
	CUtlVector< DebugFilter > m_debugFilterList;

	INextBot *m_selectedBot;						// selected bot for further debug operations

};

inline int NextBotManager::GetNextBotCount( void ) const
{
	return m_botList.Count();
}

inline void NextBotManager::Select( INextBot *bot )
{
	m_selectedBot = bot;
}

inline void NextBotManager::DeselectAll( void )
{
	m_selectedBot = NULL;
}

inline INextBot *NextBotManager::GetSelected( void ) const
{
	return m_selectedBot;
}

#endif // _NEXT_BOT_MANAGER_H_

