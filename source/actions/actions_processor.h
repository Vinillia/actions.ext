#pragma once

#ifndef _INCLUDE_ACTIONS_PROCESSOR_H
#define _INCLUDE_ACTIONS_PROCESSOR_H

#include "actions_helpers.h"
#include "sourcehook.h"

static ConVar ext_actions_debug_processors("ext_actions_debug_processors", "-2", 0, "Logs processors. -2 - Disabled, -1 - Debug all, N - function vtable index to debug");
static ConVar ext_actions_debug_post("ext_actions_debug_post", "0", 0, "Allow to log post processors. 1 - to allow, 0 - to block");

template<typename T>
static void CheckActionResult(Action<void>* action, T& result)
{
	if (!result.IsRequestingChange())
		return;
	
	if (ext_actions_debug.GetBool())
	{
		cell_t actor = gamehelpers->EntityToBCompatRef((CBaseEntity*)action->GetActor());
		const char* classname = gamehelpers->GetEntityClassname((CBaseEntity*)action->GetActor());
	
		if(result.IsDone())
		{
			if (action->GetActionBuriedUnderMe())
				LOG("%i(%s): %s is done because '%s'. Continue suspended action %s.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN", action->GetActionBuriedUnderMe()->GetName());
			else
				LOG("%i(%s): %s is done because '%s'.", actor, classname, action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else if (result.m_type == SUSPEND_FOR)
		{
			LOG("%i(%s): %s suspended for %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
		else
		{
			LOG("%i(%s): %s changed to %s because '%s'.", actor, classname, action->GetName(), result.m_action->GetName(), result.m_reason ? result.m_reason : "NO REASON GIVEN");
		}
	}

	if (!result.IsDone())
		ExecuteProcessor(static_cast<CBaseEntity*>(action->GetActor()), result.m_action);
	
	if (result.m_type != SUSPEND_FOR)
		g_pActionsManager->Remove(action);
}

template<int32_t unique, typename retn, typename... Args>
struct HandlerProcessor
{
	inline static int32_t vtableindex;
	inline static const char* name;

	static retn Process(Args... arg)
	{
		Action<void>* action = META_IFACEPTR(Action<void>);
		CBaseEntity* actor = static_cast<CBaseEntity*>(action->GetActor());

		if (IsDebugging(vtableindex, false))
		{
			LOG("%s -> %s(%i)", action->GetName(), name, vtableindex);
		}

		if constexpr (std::is_void<retn>::value)
		{
		#ifndef __linux__
			if (vtableindex == 0)
		#else
			if (vtableindex == 1)
		#endif
			{
				g_pActionsManager->Remove(action);
			}
			else
			{
				void* returnValue = NULL;
				ResultType result = g_pActionsPropagatePre->ProcessHandler(vtableindex, action, &returnValue, std::forward<Args>(arg)...);

				if (result > Pl_Continue)
				{
					RETURN_META(MRES_SUPERCEDE);
				}

				RETURN_META(MRES_IGNORED);
			}
		}
		else
		{
			retn returnValue = retn()/* META_RESULT_ORIG_RET(retn) */, originalReturn;
			originalReturn = returnValue;

			if constexpr (std::is_same<retn, ActionResult<void>>::value || std::is_same<retn, EventDesiredResult<void>>::value)
				g_pActionsManager->SetRuntimeAction(NULL);

			ResultType result = g_pActionsPropagatePre->ProcessHandler(vtableindex, action, &returnValue, std::forward<Args>(arg)...);

			if (result == Pl_Continue)
			{
				returnValue = originalReturn;
			}

			if constexpr (std::is_same<retn, Action<void>*>::value)
			{
				if (result >= Pl_Changed)
				{
					RETURN_META_VALUE(MRES_SUPERCEDE, returnValue);
				}

				RETURN_META_VALUE(MRES_IGNORED, returnValue);
			}
			else
			{
				if constexpr (std::is_same<retn, ActionResult<void>>::value || std::is_same<retn, EventDesiredResult<void>>::value)
				{
					CheckActionResult(action, returnValue);
				}

				if (result == Pl_Handled)
				{
					RETURN_META_VALUE(MRES_SUPERCEDE, returnValue);
				}
				else if (result == Pl_Changed)
				{
					RETURN_META_VALUE(MRES_OVERRIDE, returnValue);
				}

				RETURN_META_VALUE(MRES_IGNORED, returnValue);
			}
		}
	}

	static retn ProcessPost(Args... arg)
	{
		Action<void>* action = META_IFACEPTR(Action<void>);
		CBaseEntity* actor = static_cast<CBaseEntity*>(action->GetActor());

		if (IsDebugging(vtableindex, true))
		{
			LOG("%s -> %sPost(%i)", action->GetName(), name, vtableindex);
		}

		if constexpr (std::is_void<retn>::value)
		{
		#ifndef __linux__
			if (vtableindex != 0)
		#else
			if (vtableindex != 1)
		#endif
			{
				void* returnValue = NULL;
				ResultType result = g_pActionsPropagatePost->ProcessHandler(vtableindex, action, &returnValue, std::forward<Args>(arg)...);

				if (result > Pl_Continue)
				{
					RETURN_META(MRES_SUPERCEDE);
				}

				RETURN_META(MRES_IGNORED);
			}
		}
		else
		{
			retn returnValue = META_RESULT_ORIG_RET(retn), originalReturn;
			originalReturn = returnValue;

			if constexpr (std::is_same<retn, ActionResult<void>>::value || std::is_same<retn, EventDesiredResult<void>>::value)
				g_pActionsManager->SetRuntimeAction(returnValue.m_action);

			ResultType result = g_pActionsPropagatePost->ProcessHandler(vtableindex, action, &returnValue, std::forward<Args>(arg)...);

			if (result == Pl_Continue)
				returnValue = originalReturn;

			if constexpr (std::is_same<retn, Action<void>*>::value)
			{
				Action<void>* originalAction = META_RESULT_ORIG_RET(retn);

				if (originalAction && result == Pl_Continue)
				{
					ExecuteProcessor(actor, originalAction);
				}
				else
				{
					if (originalAction)
						delete originalAction;

					if (result == Pl_Handled)
					{
						RETURN_META_VALUE(MRES_SUPERCEDE, NULL);
					}
					else if (result == Pl_Changed)
					{
						if (returnValue)
							ExecuteProcessor(actor, returnValue);

						RETURN_META_VALUE(MRES_SUPERCEDE, returnValue);
					}
				}

				RETURN_META_VALUE(MRES_IGNORED, originalAction);
			}
			else 
			{
				if constexpr (std::is_same<retn, ActionResult<void>>::value || std::is_same<retn, EventDesiredResult<void>>::value)
				{
					CheckActionResult(action, returnValue);
				}

				if (result == Pl_Handled)
				{
					RETURN_META_VALUE(MRES_SUPERCEDE, returnValue);
				}
				else if (result == Pl_Changed)
				{
					RETURN_META_VALUE(MRES_OVERRIDE, returnValue);
				}

				RETURN_META_VALUE(MRES_IGNORED, returnValue);
			}
		}
	}

	static const bool IsDebugging(const int vtableindex, const bool post)
	{
		if (post && !ext_actions_debug_post.GetBool())
			return false;

		if (ext_actions_debug_processors.GetInt() == vtableindex)
			return true;

		return ext_actions_debug_processors.GetInt() == -1;
	}
};

#endif // _INCLUDE_ACTIONS_PROCESSOR_H