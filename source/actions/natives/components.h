#ifndef ACTIONS_NATIVE_COMPONENTS_H
#define ACTIONS_NATIVE_COMPONENTS_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static inline cell_t native_component_create(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context, entity_index = params[1]](
				CBaseEntity* entity,
				IPluginFunction* initial,
				IPluginFunction* update,
				IPluginFunction* upkeep,
				IPluginFunction* reset,
				const char* name) -> cell_t
			{
				INextBot* nextbot = g_pActionsTools->MyNextBotPointer(entity);
				if (!nextbot)
				{
					context->ReportError("Failed to get entity nextbot ptr %i", entity_index);
					return 0;
				}

				ActionComponent* component = new ActionComponent(nextbot, context, initial, name);
				if (component->HasHandleError())
				{
					context->ReportError("Failed to create handle (error %i)", component->GetHandleError());
					return 0;
				}

				component->SetUpdateCallback(update);
				component->SetUpkeepCallback(upkeep);
				component->SetResetCallback(reset);
				return static_cast<cell_t>(component->GetHandle());
			},
			native_entity_extractor,
			native_nullable_function_extractor,
			native_nullable_function_extractor,
			native_nullable_function_extractor,
			native_nullable_function_extractor,
			native_nullable_string_extractor);
	}

	static inline cell_t native_component_get_address(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component) -> cell_t
			{
				return ToPseudoAddress(component);
			},
			native_component_extractor);
	}

	static inline cell_t native_component_get_actor(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionComponent* component) -> cell_t
			{
				INextBot* bot = component->GetBot();
				if (!bot)
					return -1;

				CBaseEntity* entity = g_pActionsTools->GetEntity(bot);
				if (!entity)
				{
					context->ReportError("Failed to get entity from nextbot ptr");
					return 0;
				}

				const int entindex = gamehelpers->EntityToBCompatRef(entity);
				return entindex == -1 ? 0 : static_cast<cell_t>(entindex);
			},
			native_component_extractor);
	}

	static inline cell_t native_component_get_name(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionComponent* component, cell_t output, cell_t max_length) -> cell_t
			{
				const char* name = component->GetName();
				if (!name)
				{
					context->StringToLocal(output, max_length, "null");
					return -1;
				}

				return context->StringToLocal(output, max_length, name);
			},
			native_component_extractor,
			native_cell_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_component_set_name(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component, const char* name) -> cell_t
			{
				component->SetName(name);
				return 0;
			},
			native_owned_component_extractor,
			native_string_extractor);
	}

	static inline cell_t native_component_get_current_action(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component) -> cell_t
			{
				return ToPseudoAddress(component->CurrentAction());
			},
			native_owned_component_extractor);
	}

	static inline cell_t native_component_set_update_callback(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component, IPluginFunction* function) -> cell_t
			{
				component->SetUpdateCallback(function);
				return 0;
			},
			native_component_extractor,
			native_function_extractor);
	}

	static inline cell_t native_component_set_upkeep_callback(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component, IPluginFunction* function) -> cell_t
			{
				component->SetUpkeepCallback(function);
				return 0;
			},
			native_component_extractor,
			native_function_extractor);
	}

	static inline cell_t native_component_set_reset_callback(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionComponent* component, IPluginFunction* function) -> cell_t
			{
				component->SetResetCallback(function);
				return 0;
			},
			native_component_extractor,
			native_function_extractor);
	}
}

#endif // ACTIONS_NATIVE_COMPONENTS_H
