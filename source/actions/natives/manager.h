#ifndef ACTIONS_NATIVE_MANAGER_H
#define ACTIONS_NATIVE_MANAGER_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static inline bool change_action_listener(
		IPluginContext* context,
		nb_action_ptr action,
		HashValue hash,
		IPluginFunction* function,
		bool post,
		bool remove)
	{
		ActionPropagation* propagation = post ? &g_actionsPropagationPost : &g_actionsPropagationPre;

		if (!function)
			return propagation->RemoveListener(action, hash, context);

		if (remove)
			return propagation->RemoveListener(action, hash, function);

		return propagation->AddListener(action, hash, function);
	}

	static inline cell_t native_action_set_listener(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr action, HashValue hash, IPluginFunction* function, bool post) -> cell_t
			{
				return static_cast<cell_t>(change_action_listener(
					context,
					action,
					hash,
					function,
					post,
					false));
			},
			native_action_extractor,
			native_hash_extractor,
			native_nullable_function_extractor,
			native_bool_extractor);
	}

	static inline cell_t native_action_remove_listener(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](nb_action_ptr action, HashValue hash, IPluginFunction* function, bool post) -> cell_t
			{
				return static_cast<cell_t>(change_action_listener(
					context,
					action,
					hash,
					function,
					post,
					true));
			},
			native_action_extractor,
			native_hash_extractor,
			native_nullable_function_extractor,
			native_bool_extractor);
	}

	static inline cell_t native_manager_register_action_id(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](const char* name) -> cell_t
			{
				return static_cast<cell_t>(g_actionsManager.RegisterActionID(name));
			},
			native_string_extractor);
	}

	static inline cell_t native_manager_find_action_id(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](const char* name) -> cell_t
			{
				return static_cast<cell_t>(g_actionsManager.FindActionID(name));
			},
			native_string_extractor);
	}

	static inline cell_t native_manager_lookup_entity_action_by_id(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](CBaseEntity* entity, cell_t id) -> cell_t
			{
				return ToPseudoAddress(g_actionsManager.LookupEntityAction(entity, id));
			},
			native_nextbot_entity_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_manager_lookup_entity_action_by_name(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](CBaseEntity* entity, const char* name) -> cell_t
			{
				return ToPseudoAddress(g_actionsManager.LookupEntityAction(entity, name));
			},
			native_nextbot_entity_extractor,
			native_string_extractor);
	}
}

#endif // ACTIONS_NATIVE_MANAGER_H
