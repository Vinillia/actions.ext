#ifndef _INCLUDE_NEXTBOT_BEHAVIOR_SHARED_H
#define _INCLUDE_NEXTBOT_BEHAVIOR_SHARED_H

#include "actionsdefs.h"

#define PROCESS_EVENT( METHOD )								\
		{													\
			if ( !m_isStarted )								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while( _action )								\
			{												\
				behavior::ProcessEvent_DebugReceive(m_actor, m_behavior, _action, #METHOD); \
				_result = _action->METHOD( m_actor );	\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if ( _action )									\
			{												\
				behavior::ProcessEvent_DebugRespond(m_actor, m_behavior, _action, #METHOD, _result); \
				_action->StorePendingEventResult( _result, #METHOD );	\
			}												\
															\
			INextBotEventResponder::METHOD();			\
		}


#define PROCESS_EVENT_WITH_1_ARG( METHOD, ARG1 )		\
		{													\
			if ( !m_isStarted )								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while( _action )								\
			{												\
				behavior::ProcessEvent_DebugReceive(m_actor, m_behavior, _action, #METHOD); \
				_result = _action->METHOD( m_actor, ARG1 );		\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();		\
			}												\
															\
			if ( _action )									\
			{												\
				behavior::ProcessEvent_DebugRespond(m_actor, m_behavior, _action, #METHOD, _result); \
				_action->StorePendingEventResult( _result, #METHOD );	\
			}												\
															\
			INextBotEventResponder::METHOD( ARG1 );		\
		}


#define PROCESS_EVENT_WITH_2_ARGS( METHOD, ARG1, ARG2 )	\
		{													\
			if ( !m_isStarted )								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while( _action )								\
			{												\
				behavior::ProcessEvent_DebugReceive(m_actor, m_behavior, _action, #METHOD); \
				_result = _action->METHOD( m_actor, ARG1, ARG2 );		\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();				\
			}												\
															\
			if ( _action )									\
			{												\
				behavior::ProcessEvent_DebugRespond(m_actor, m_behavior, _action, #METHOD, _result); \
				_action->StorePendingEventResult( _result, #METHOD );	\
			}												\
															\
			INextBotEventResponder::METHOD( ARG1, ARG2 );			\
		}


#define PROCESS_EVENT_WITH_3_ARGS( METHOD, ARG1, ARG2, ARG3 )	\
		{													\
			if ( !m_isStarted )								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while( _action )								\
			{												\
				behavior::ProcessEvent_DebugReceive(m_actor, m_behavior, _action, #METHOD); \
				_result = _action->METHOD( m_actor, ARG1, ARG2, ARG3 );		\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();				\
			}												\
															\
			if ( _action )									\
			{												\
				behavior::ProcessEvent_DebugRespond(m_actor, m_behavior, _action, #METHOD, _result); \
				_action->StorePendingEventResult( _result, #METHOD );	\
			}												\
															\
			INextBotEventResponder::METHOD( ARG1, ARG2, ARG3 );			\
		}


#define PROCESS_EVENT_WITH_4_ARGS( METHOD, ARG1, ARG2, ARG3, ARG4 )	\
		{													\
			if ( !m_isStarted )								\
				return;										\
															\
			Action< Actor > *_action = this;				\
			EventDesiredResult< Actor > _result;			\
															\
			while( _action )								\
			{												\
				behavior::ProcessEvent_DebugReceive(m_actor, m_behavior, _action, #METHOD); \
				_result = _action->METHOD( m_actor, ARG1, ARG2, ARG3, ARG4 );		\
				if ( !_result.IsContinue() )				\
					break;									\
				_action = _action->GetActionBuriedUnderMe();				\
			}												\
															\
			if ( _action )									\
			{												\
				behavior::ProcessEvent_DebugRespond(m_actor, m_behavior, _action, #METHOD, _result); \
				_action->StorePendingEventResult( _result, #METHOD );	\
			}												\
															\
			INextBotEventResponder::METHOD( ARG1, ARG2, ARG3, ARG4 );			\
		}

class INextBot;

template<typename T>
struct ActionResult;

template<typename T>
struct EventDesiredResult;

namespace behavior
{
	bool IsDebuggingHistory();
	void Debug(INextBot* me, nb_action_ptr action, nb_behavior_ptr behavior, const char* invokeStr);
	void Debug(CBaseEntity* me, nb_action_ptr action, nb_behavior_ptr behavior, const char* invokeStr);

	void ProcessEvent_DebugReceive(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method);
	void ProcessEvent_DebugRespond(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method, const ActionResult<CBaseEntity>& result);
	void ProcessEvent_DebugRespond(CBaseEntity* actor, nb_behavior_ptr behavior, nb_action_ptr action, const char* method, const EventDesiredResult<CBaseEntity>& result);

	void ApplyResult_Debug_ChangeTo(CBaseEntity* me, nb_action_ptr action, nb_action_ptr newAction, nb_behavior_ptr behavior, const ActionResult<CBaseEntity>& result);
	void ApplyResult_Debug_SuspendFor(CBaseEntity* me, nb_action_ptr action, nb_action_ptr newAction, nb_action_ptr topAction, nb_behavior_ptr behavior, const ActionResult<CBaseEntity>& result);
	void ApplyResult_Debug_Done(CBaseEntity* me, nb_action_ptr action, nb_action_ptr newAction, nb_action_ptr resumedAction, nb_behavior_ptr behavior, const ActionResult<CBaseEntity>& result);
}

#endif // !_INCLUDE_NEXTBOT_BEHAVIOR_SHARED_H
