#pragma once

#include <utility>
#include <map>

#include "utils.h"

#include "extension.h"
#include "sourcehook.h"

#include "actions_manager.h"
#include "actions_propagate.h"

#include "NextBotBehavior.h"
#include "NextBotIntentionInterface.h"

extern std::map<std::string, size_t>& GetOffsetsInfo();

extern void HookIntentions(IGameConfig* config);
extern void ReconfigureHooks();

#define DEFINE_PROCESSOR(unique, name, ret, ...)	\
	HandlerProcessor<unique, ret, ##__VA_ARGS__> name

#define START_PROCESSOR(hookname, varname)	\
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(action), SH_STATIC(decltype(varname)::Process), false); \
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(action), SH_STATIC(decltype(varname)::ProcessPost), true); \
	varname.vtableindex = offsets[#hookname]; \
	varname.name = #hookname

class ActionProcessor;
static void CreateActionProcessor(CBaseEntity* entity, Action<void>* action);

template<typename T>
static void CheckActionResult(Action<void>* action, T& result)
{
	if (!result.IsRequestingChange())
		return;

	if (!result.IsDone())
		CreateActionProcessor(static_cast<CBaseEntity*>(action->GetActor()), result.m_action);

	if (result.m_type != SUSPEND_FOR)
		g_pActionsManager->Remove(action);
}

template<size_t unique, typename retn, typename... Args>
struct HandlerProcessor
{
	inline static size_t vtableindex;
	inline static const char* name;

	static retn Process(Args... arg)
	{
		Action<void>* action = META_IFACEPTR(Action<void>);
		CBaseEntity* actor = static_cast<CBaseEntity*>(action->GetActor());

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
			retn returnValue = META_RESULT_ORIG_RET(retn), originalReturn;
			originalReturn = returnValue;

			if constexpr (std::is_same<retn, ActionResult<void>>::value || std::is_same<retn, EventDesiredResult<void>>::value)
				g_pActionsManager->SetRuntimeAction(NULL);

			ResultType result = g_pActionsPropagatePre->ProcessHandler(vtableindex, action, &returnValue, std::forward<Args>(arg)...);

			if (result == Pl_Continue)
				returnValue = originalReturn;

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
					CreateActionProcessor(actor, originalAction);
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
							CreateActionProcessor(actor, returnValue);

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
};

class ActionProcessor
{
	inline static std::vector<std::string> m_hookedNames;

public:
	ActionProcessor(CBaseEntity* entity, Action<void>* action);
	ActionProcessor(Action<void>* action);

	~ActionProcessor() = default;
public:
	Action<void>* m_action;

#pragma region processors
	#ifndef __linux__
		DEFINE_PROCESSOR(0, dctor, void, bool);
	#else
		DEFINE_PROCESSOR(0, dctor, void);
	#endif

	DEFINE_PROCESSOR(1, start, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(12, update, ActionResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(13, end, void, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(14, suspend, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(15, resume, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(16, initialAction, Action<void>*, CBaseEntity*);
	DEFINE_PROCESSOR(17, leaveGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(18, landGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(19, contact, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CGameTrace*);
	DEFINE_PROCESSOR(10, movetoSuccess, EventDesiredResult<void>, CBaseEntity*, const Path*);
	DEFINE_PROCESSOR(20, movetoFailure, EventDesiredResult<void>, CBaseEntity*, const Path*, MoveToFailureType);
	DEFINE_PROCESSOR(21, stuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(22, unstuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(23, postureChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(24, animationActivityComplete, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(25, animationActivityInterrupted, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(26, animationEvent, EventDesiredResult<void>, CBaseEntity*, animevent_t*);
	DEFINE_PROCESSOR(27, ignite, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(28, injured, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(29, killed, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(30, otherKilled, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(31, sight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(32, lostSight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(33, threatChanged, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(34, sound, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const Vector&, KeyValues*);
	DEFINE_PROCESSOR(35, spokeConcept, EventDesiredResult<void>, CBaseEntity*, CBaseCombatCharacter*, AIConcept_t, AI_Response*, void*);
	DEFINE_PROCESSOR(36, navareaChanged, EventDesiredResult<void>, CBaseEntity*, CNavArea*, CNavArea*);
	DEFINE_PROCESSOR(37, modelChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(38, pickup, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(39, drop, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(40, shoved, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(41, blinded, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(42, commandAttack, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(43, commandApproachVector, EventDesiredResult<void>, CBaseEntity*, const Vector&, float);
	DEFINE_PROCESSOR(44, commandApproachEntity, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(45, commandRetreat, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, float);
	DEFINE_PROCESSOR(46, commandPause, EventDesiredResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(47, commandResume, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(48, abletoBlock, bool, const INextBot*);

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		DEFINE_PROCESSOR(49, enteredSpit, EventDesiredResult<void>, CBaseEntity*);
		DEFINE_PROCESSOR(50, hitVomitjar, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
		DEFINE_PROCESSOR(51, commandAssault, EventDesiredResult<void>, CBaseEntity*);
		DEFINE_PROCESSOR(52, commandString, EventDesiredResult<void>, CBaseEntity*, const char*);
	#endif
#pragma endregion processors
};

static void CreateActionProcessor(CBaseEntity* entity, Action<void>* action)
{
	ActionProcessor processor(entity, action);
}