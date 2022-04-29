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

#define DEFINE_PROCESSOR(name, ret, ...)	\
	HandlerProcessor<ret, ##__VA_ARGS__> name

#define START_PROCESSOR(hookname, varname)	\
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(action), SH_STATIC(decltype(varname)::Process), false); \
	SH_ADD_MANUALVPHOOK(hookname, static_cast<void*>(action), SH_STATIC(decltype(varname)::ProcessPost), true); \
	varname.vtableindex = offsets[#hookname]; \
	varname.name = #hookname

class ActionProcessor;
static void CreateActionProcessor(CBaseEntity* entity, Action<void>* action);

template<typename Q>
static void CheckActionResult(Action<void>* action, Q& result)
{
	if (!result.IsRequestingChange())
		return;

	if (!result.IsDone())
		CreateActionProcessor(static_cast<CBaseEntity*>(action->GetActor()), result.m_action);

	if (result.m_type != SUSPEND_FOR)
		g_pActionsManager->Remove(action);
}

template<typename retn, typename... Args>
struct HandlerProcessor
{
	inline static size_t vtableindex;
	inline static const char* name;

	static retn Process(Args... arg)
	{
		Action<void>* action = META_IFACEPTR(Action<void>);
		CBaseEntity* actor = static_cast<CBaseEntity*>(action->GetActor());

		g_pActionsManager->SetRuntimeAction(action);

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
		}
		else
		{
			retn returnValue = META_RESULT_ORIG_RET(retn), originalReturn;
			originalReturn = returnValue;

			ResultType result = g_pActionsPropagatePre->ProcessHandler(vtableindex, &returnValue, std::forward<Args>(arg)...);

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
			return;
		}
		else
		{
			retn returnValue = META_RESULT_ORIG_RET(retn), originalReturn;
			originalReturn = returnValue;

			ResultType result = g_pActionsPropagatePost->ProcessHandler(vtableindex, &returnValue, std::forward<Args>(arg)...);

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
		DEFINE_PROCESSOR(dctor, void, bool);
	#else
		DEFINE_PROCESSOR(dctor, void);
	#endif

	DEFINE_PROCESSOR(start, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(update, ActionResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(end, void, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(suspend, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(resume, ActionResult<void>, CBaseEntity*, Action<void>*);
	DEFINE_PROCESSOR(initialAction, Action<void>*, CBaseEntity*);
	DEFINE_PROCESSOR(leaveGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(landGround, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(contact, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CGameTrace*);
	DEFINE_PROCESSOR(movetoSuccess, EventDesiredResult<void>, CBaseEntity*, const Path*);
	DEFINE_PROCESSOR(movetoFailure, EventDesiredResult<void>, CBaseEntity*, const Path*, MoveToFailureType);
	DEFINE_PROCESSOR(stuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(unstuck, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(postureChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(animationActivityComplete, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(animationActivityInterrupted, EventDesiredResult<void>, CBaseEntity*, int);
	DEFINE_PROCESSOR(animationEvent, EventDesiredResult<void>, CBaseEntity*, animevent_t*);
	DEFINE_PROCESSOR(ignite, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(injured, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(killed, EventDesiredResult<void>, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(otherKilled, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const CTakeDamageInfo*);
	DEFINE_PROCESSOR(sight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(lostSight, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(threatChanged, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(sound, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, const Vector&, KeyValues*);
	DEFINE_PROCESSOR(spokeConcept, EventDesiredResult<void>, CBaseEntity*, CBaseCombatCharacter*, AIConcept_t, AI_Response*, void*);
	DEFINE_PROCESSOR(navareaChanged, EventDesiredResult<void>, CBaseEntity*, CNavArea*, CNavArea*);
	DEFINE_PROCESSOR(modelChanged, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(pickup, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(drop, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(shoved, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(blinded, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(commandAttack, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(commandApproachVector, EventDesiredResult<void>, CBaseEntity*, const Vector&, float);
	DEFINE_PROCESSOR(commandApproachEntity, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
	DEFINE_PROCESSOR(commandRetreat, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*, float);
	DEFINE_PROCESSOR(commandPause, EventDesiredResult<void>, CBaseEntity*, float);
	DEFINE_PROCESSOR(commandResume, EventDesiredResult<void>, CBaseEntity*);
	DEFINE_PROCESSOR(abletoBlock, bool, const INextBot*);

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		DEFINE_PROCESSOR(enteredSpit, EventDesiredResult<void>, CBaseEntity*);
		DEFINE_PROCESSOR(hitVomitjar, EventDesiredResult<void>, CBaseEntity*, CBaseEntity*);
		DEFINE_PROCESSOR(commandAssault, EventDesiredResult<void>, CBaseEntity*);
		DEFINE_PROCESSOR(commandString, EventDesiredResult<void>, CBaseEntity*, const char*);
	#endif
#pragma endregion processors
};

static void CreateActionProcessor(CBaseEntity* entity, Action<void>* action)
{
	ActionProcessor processor(entity, action);
}