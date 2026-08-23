#ifndef ACTIONS_NATIVE_BEHAVIOR_H
#define ACTIONS_NATIVE_BEHAVIOR_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static inline cell_t native_behavior_action_get_name(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr action, cell_t output, cell_t max_length) -> cell_t
			{
				return context->StringToLocal(output, max_length, action->GetName());
			},
			native_action_extractor,
			native_cell_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_behavior_action_get_parent(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return ToPseudoAddress(action->m_parent);
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_get_child(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return ToPseudoAddress(action->GetActiveChildAction());
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_get_under(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return ToPseudoAddress(action->GetActionBuriedUnderMe());
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_get_above(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return ToPseudoAddress(action->GetActionCoveringMe());
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_get_actor(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return gamehelpers->EntityToBCompatRef(g_actionsManager.GetActionActor(action));
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_get_suspended(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return static_cast<cell_t>(action->m_isSuspended);
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_set_suspended(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, bool suspended) -> cell_t
			{
				const bool previous = action->m_isSuspended;
				action->m_isSuspended = suspended;
				return static_cast<cell_t>(previous);
			},
			native_action_extractor,
			native_bool_extractor);
	}

	static inline cell_t native_behavior_action_get_started(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				return static_cast<cell_t>(action->m_isStarted);
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_set_started(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, bool started) -> cell_t
			{
				const bool previous = action->m_isStarted;
				action->m_isStarted = started;
				return static_cast<cell_t>(previous);
			},
			native_action_extractor,
			native_bool_extractor);
	}

	static inline cell_t native_behavior_action_set_handle_entity(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, cell_t offset, CBaseEntity* entity) -> cell_t
			{
				CBaseHandle& handle = *reinterpret_cast<CBaseHandle*>(
					reinterpret_cast<uintptr_t>(action) + static_cast<uint32_t>(offset));

				if (entity)
					handle = reinterpret_cast<IHandleEntity*>(entity)->GetRefEHandle();
				else
					handle = INVALID_EHANDLE_INDEX;

				return 0;
			},
			native_action_extractor,
			native_cell_extractor,
			native_nullable_entity_extractor);
	}

	static inline cell_t native_behavior_action_get_handle_entity(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, cell_t offset) -> cell_t
			{
				const CBaseHandle& handle = *reinterpret_cast<const CBaseHandle*>(
					reinterpret_cast<uintptr_t>(action) + static_cast<uint32_t>(offset));

				return static_cast<cell_t>(handle.GetEntryIndex());
			},
			native_action_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_behavior_action_continue(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr) -> cell_t
			{
				ActionResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_continue_result();
				return Pl_Changed;
			},
			native_action_extractor);
	}

	static inline cell_t native_behavior_action_change_to(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, nb_action_ptr target, const char* reason) -> cell_t
			{
				ActionResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_change_to_result(target, reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_action_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_suspend_for(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, nb_action_ptr target, const char* reason) -> cell_t
			{
				ActionResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				if (nb_action_ptr runtime_action = g_actionsManager.GetRuntimeAction())
					runtime_action->m_eventResult = make_try_continue_result(RESULT_NONE);

				*result = make_suspend_for_result(target, reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_action_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_done(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, const char* reason) -> cell_t
			{
				ActionResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_done_result(reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_try_continue(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, cell_t priority) -> cell_t
			{
				EventDesiredResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeDesiredResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_try_continue_result(static_cast<EventResultPriorityType>(priority));
				return Pl_Changed;
			},
			native_action_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_behavior_action_try_change_to(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, nb_action_ptr target, cell_t priority, const char* reason) -> cell_t
			{
				EventDesiredResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeDesiredResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_try_change_to_result(
					target,
					static_cast<EventResultPriorityType>(priority),
					reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_action_extractor,
			native_cell_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_try_suspend_for(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, nb_action_ptr target, cell_t priority, const char* reason) -> cell_t
			{
				EventDesiredResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeDesiredResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_try_suspend_for_result(
					target,
					static_cast<EventResultPriorityType>(priority),
					reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_action_extractor,
			native_cell_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_try_done(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, cell_t priority, const char* reason) -> cell_t
			{
				EventDesiredResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeDesiredResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_try_done_result(static_cast<EventResultPriorityType>(priority), reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_cell_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_try_sustain(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr, cell_t priority, const char* reason) -> cell_t
			{
				EventDesiredResult<CBaseEntity>* result = g_actionsManager.GetActionRuntimeDesiredResult();
				if (!result)
				{
					context->ReportError("Attempt to access an invalid result.");
					return -1;
				}

				*result = make_try_sustain_result(static_cast<EventResultPriorityType>(priority), reason);
				return Pl_Changed;
			},
			native_action_extractor,
			native_cell_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_behavior_action_store_pending_event_result(IPluginContext*, const cell_t*)
	{
		return 0;
	}
}

#endif // ACTIONS_NATIVE_BEHAVIOR_H
