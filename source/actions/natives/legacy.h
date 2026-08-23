#ifndef ACTIONS_NATIVE_LEGACY_H
#define ACTIONS_NATIVE_LEGACY_H

#include "actions/natives/context.h"
#include "actions/legacy/custom_legacy.h"

#include <cstring>
#include <vector>

namespace actions_natives
{
	static inline cell_t native_legacy_allocate(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](cell_t size) -> cell_t
			{
				return ToPseudoAddress(::operator new(static_cast<size_t>(size)));
			},
			native_cell_extractor);
	}

	static inline cell_t native_legacy_deallocate(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](nb_action_ptr action) -> cell_t
			{
				delete action;
				return 0;
			},
			native_action_extractor);
	}

	static inline cell_t native_legacy_iterate_entity_actions(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](CBaseEntity* entity, IPluginFunction* iterator) -> cell_t
			{
				std::vector<nb_action_ptr> actions;
				if (!g_pActionsTools->GetEntityActions(entity, actions))
					throw format_exception("failed to get actions for entity");

				for (nb_action_ptr action : actions)
				{
					iterator->PushCell(ToPseudoAddress(action));
					iterator->Execute(nullptr);
				}

				return static_cast<cell_t>(actions.size());
			},
			native_entity_extractor,
			native_function_extractor);
	}

	static inline cell_t native_legacy_find_entity_action(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](CBaseEntity* entity, const char* name) -> cell_t
			{
				std::vector<nb_action_ptr> actions;
				if (!g_pActionsTools->GetEntityActions(entity, actions))
					throw format_exception("failed to get actions for entity");

				for (nb_action_ptr action : actions)
				{
					if (std::strcmp(action->GetName(), name) == 0)
						return ToPseudoAddress(action);
				}

				return 0;
			},
			native_entity_extractor,
			native_string_extractor);
	}

	static inline cell_t native_legacy_create_action(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](const char* name) -> cell_t
			{
				ActionCustomLegacy* action = new ActionCustomLegacy(name);
				g_actionsManager.AddPending(reinterpret_cast<nb_action_ptr>(action));
				return ToPseudoAddress(action);
			},
			native_string_extractor);
	}
}

#endif // ACTIONS_NATIVE_LEGACY_H
