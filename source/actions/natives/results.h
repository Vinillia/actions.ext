#pragma once

#ifndef ACTIONS_NATIVE_RESULTS_H
#define ACTIONS_NATIVE_RESULTS_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static inline ActionResult<CBaseEntity> make_continue_result()
	{
		return ActionResult<CBaseEntity>(CONTINUE, nullptr, nullptr);
	}

	static inline ActionResult<CBaseEntity> make_change_to_result(nb_action_ptr action, const char* reason)
	{
		return ActionResult<CBaseEntity>(CHANGE_TO, action, reason);
	}

	static inline ActionResult<CBaseEntity> make_suspend_for_result(nb_action_ptr action, const char* reason)
	{
		return ActionResult<CBaseEntity>(SUSPEND_FOR, action, reason);
	}

	static inline ActionResult<CBaseEntity> make_done_result(const char* reason)
	{
		return ActionResult<CBaseEntity>(DONE, nullptr, reason);
	}

	static inline EventDesiredResult<CBaseEntity> make_try_continue_result(EventResultPriorityType priority)
	{
		return EventDesiredResult<CBaseEntity>(CONTINUE, nullptr, priority);
	}

	static inline EventDesiredResult<CBaseEntity> make_try_change_to_result(
		nb_action_ptr action,
		EventResultPriorityType priority,
		const char* reason)
	{
		return EventDesiredResult<CBaseEntity>(CHANGE_TO, action, priority, reason);
	}

	static inline EventDesiredResult<CBaseEntity> make_try_suspend_for_result(
		nb_action_ptr action,
		EventResultPriorityType priority,
		const char* reason)
	{
		return EventDesiredResult<CBaseEntity>(SUSPEND_FOR, action, priority, reason);
	}

	static inline EventDesiredResult<CBaseEntity> make_try_done_result(
		EventResultPriorityType priority,
		const char* reason = nullptr)
	{
		return EventDesiredResult<CBaseEntity>(DONE, nullptr, priority, reason);
	}

	static inline EventDesiredResult<CBaseEntity> make_try_sustain_result(
		EventResultPriorityType priority,
		const char* reason = nullptr)
	{
		return EventDesiredResult<CBaseEntity>(SUSTAIN, nullptr, priority, reason);
	}

	static inline cell_t native_action_result_get_reason(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result, cell_t output, cell_t max_length) -> cell_t
			{
				if (!result->m_reason)
					return -1;

				return context->StringToLocal(output, max_length, result->m_reason);
			},
			native_action_result_extractor,
			native_cell_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_action_result_set_reason(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result, const char* reason) -> cell_t
			{
				result->m_reason = reason;
				return 1;
			},
			native_action_result_extractor,
			native_string_extractor);
	}

	static inline cell_t native_action_result_get_type(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result) -> cell_t
			{
				return static_cast<cell_t>(result->m_type);
			},
			native_action_result_extractor);
	}

	static inline cell_t native_action_result_set_type(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result, cell_t type) -> cell_t
			{
				const ActionResultType previous = result->m_type;
				result->m_type = static_cast<ActionResultType>(type);
				return static_cast<cell_t>(previous);
			},
			native_action_result_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_action_result_get_action(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result) -> cell_t
			{
				return ToPseudoAddress(result->m_action);
			},
			native_action_result_extractor);
	}

	static inline cell_t native_action_result_set_action(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionResult<CBaseEntity>* result, nb_action_ptr action) -> cell_t
			{
				nb_action_ptr previous = result->m_action;
				result->m_action = action;
				return ToPseudoAddress(previous);
			},
			native_action_result_extractor,
			native_action_extractor);
	}

	static inline cell_t native_desired_result_get_priority(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](EventDesiredResult<CBaseEntity>* result) -> cell_t
			{
				return static_cast<cell_t>(result->m_priority);
			},
			native_action_desired_result_extractor);
	}

	static inline cell_t native_desired_result_set_priority(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](EventDesiredResult<CBaseEntity>* result, cell_t priority) -> cell_t
			{
				const EventResultPriorityType previous = result->m_priority;
				result->m_priority = static_cast<EventResultPriorityType>(priority);
				return static_cast<cell_t>(previous);
			},
			native_action_desired_result_extractor,
			native_cell_extractor);
	}
}

#endif // ACTIONS_NATIVE_RESULTS_H
