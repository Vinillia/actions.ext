#ifndef ACTIONS_NATIVE_USERDATA_H
#define ACTIONS_NATIVE_USERDATA_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static inline cell_t native_manager_get_user_data_cell(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, cell_t* output) -> cell_t
			{
				cell_t value;
				if (!g_actionsManager.GetUserData(action, key, value))
					return 0;

				*output = value;
				return 1;
			},
			native_action_extractor,
			native_string_extractor,
			native_cell_ptr_extractor);
	}

	static inline cell_t native_manager_set_user_data_cell(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, cell_t value) -> cell_t
			{
				g_actionsManager.SetUserData(action, key, value);
				return 0;
			},
			native_action_extractor,
			native_string_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_manager_get_user_data_vector(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, cell_t* output) -> cell_t
			{
				Vector value;
				if (!g_actionsManager.GetUserData(action, key, value))
					return 0;

				output[0] = sp_ftoc(value.x);
				output[1] = sp_ftoc(value.y);
				output[2] = sp_ftoc(value.z);
				return 1;
			},
			native_action_extractor,
			native_string_extractor,
			native_cell_ptr_extractor);
	}

	static inline cell_t native_manager_set_user_data_vector(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, Vector value) -> cell_t
			{
				g_actionsManager.SetUserData(action, key, value);
				return 0;
			},
			native_action_extractor,
			native_string_extractor,
			native_vector_extractor);
	}

	static inline cell_t native_manager_get_user_data_string(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, char* output, cell_t max_length) -> cell_t
			{
				std::string value;
				if (!g_actionsManager.GetUserData(action, key, value))
					return 0;

				ke::SafeStrcpy(output, max_length, value.c_str());
				return 1;
			},
			native_action_extractor,
			native_string_extractor,
			native_mutable_string_extractor,
			native_cell_extractor);
	}

	static inline cell_t native_manager_set_user_data_string(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action, const char* key, const char* value) -> cell_t
			{
				g_actionsManager.SetUserData(action, key, value);
				return 0;
			},
			native_action_extractor,
			native_string_extractor,
			native_string_extractor);
	}
}

#endif // ACTIONS_NATIVE_USERDATA_H
