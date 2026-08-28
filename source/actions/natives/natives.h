#pragma once

#ifndef ACTIONS_NATIVES_H
#define ACTIONS_NATIVES_H

#include "actions/natives/context.h"
#include "actions/natives/results.h"
#include "actions/natives/behavior.h"
#include "actions/natives/userdata.h"
#include "actions/natives/components.h"
#include "actions/natives/manager.h"
#include "actions/natives/legacy.h"
#include "actions/natives/transition.h"

sp_nativeinfo_t g_actionsNatives[] =
{
	{ "__action_setlistener",                         actions_natives::native_action_set_listener },
	{ "__action_removelistener",                      actions_natives::native_action_remove_listener },

	{ "ActionsManager.RegisterActionId",              actions_natives::native_manager_register_action_id },
	{ "ActionsManager.FindActionId",                  actions_natives::native_manager_find_action_id },

	{ "ActionsManager.LookupEntityActionById",        actions_natives::native_manager_lookup_entity_action_by_id },
	{ "ActionsManager.LookupEntityActionByName",      actions_natives::native_manager_lookup_entity_action_by_name },

	{ "ActionsManager.SetActionUserData",             actions_natives::native_manager_set_user_data_cell },
	{ "ActionsManager.GetActionUserData",             actions_natives::native_manager_get_user_data_cell },

	{ "ActionsManager.SetActionUserDataString",       actions_natives::native_manager_set_user_data_string },
	{ "ActionsManager.GetActionUserDataString",       actions_natives::native_manager_get_user_data_string },

	{ "ActionsManager.SetActionUserDataVector",       actions_natives::native_manager_set_user_data_vector },
	{ "ActionsManager.GetActionUserDataVector",       actions_natives::native_manager_get_user_data_vector },

	{ "ActionComponent.ActionComponent",              actions_natives::native_component_create },
	{ "ActionComponent.Address",                      actions_natives::native_component_get_address },
	{ "ActionComponent.Actor",                        actions_natives::native_component_get_actor },
	{ "ActionComponent.GetName",                      actions_natives::native_component_get_name },
	{ "ActionComponent.SetName",                      actions_natives::native_component_set_name },
	{ "ActionComponent.CurrentAction.get",            actions_natives::native_component_get_current_action },
	{ "ActionComponent.Update.set",                   actions_natives::native_component_set_update_callback },
	{ "ActionComponent.Upkeep.set",                   actions_natives::native_component_set_upkeep_callback },
	{ "ActionComponent.Reset.set",                    actions_natives::native_component_set_reset_callback },

	{ "BehaviorAction.GetName",                       actions_natives::native_behavior_action_get_name },

	{ "BehaviorAction.StorePendingEventResult",       actions_natives::native_behavior_action_store_pending_event_result },
	{ "BehaviorAction.Continue",                      actions_natives::native_behavior_action_continue },
	{ "BehaviorAction.ChangeTo",                      actions_natives::native_behavior_action_change_to },
	{ "BehaviorAction.SuspendFor",                    actions_natives::native_behavior_action_suspend_for },
	{ "BehaviorAction.Done",                          actions_natives::native_behavior_action_done },
	{ "BehaviorAction.TryContinue",                   actions_natives::native_behavior_action_try_continue },
	{ "BehaviorAction.TryChangeTo",                   actions_natives::native_behavior_action_try_change_to },
	{ "BehaviorAction.TrySuspendFor",                 actions_natives::native_behavior_action_try_suspend_for },
	{ "BehaviorAction.TryDone",                       actions_natives::native_behavior_action_try_done },
	{ "BehaviorAction.TryToSustain",                  actions_natives::native_behavior_action_try_sustain },

	{ "BehaviorAction.Parent.get",                    actions_natives::native_behavior_action_get_parent },
	{ "BehaviorAction.Child.get",                     actions_natives::native_behavior_action_get_child },
	{ "BehaviorAction.Under.get",                     actions_natives::native_behavior_action_get_under },
	{ "BehaviorAction.Above.get",                     actions_natives::native_behavior_action_get_above },
	{ "BehaviorAction.Actor.get",                     actions_natives::native_behavior_action_get_actor },

	{ "BehaviorAction.IsSuspended.get",               actions_natives::native_behavior_action_get_suspended },
	{ "BehaviorAction.IsSuspended.set",               actions_natives::native_behavior_action_set_suspended },

	{ "BehaviorAction.IsStarted.get",                 actions_natives::native_behavior_action_get_started },
	{ "BehaviorAction.IsStarted.set",                 actions_natives::native_behavior_action_set_started },

	{ "BehaviorAction.SetHandleEntity",               actions_natives::native_behavior_action_set_handle_entity },
	{ "BehaviorAction.GetHandleEntity",               actions_natives::native_behavior_action_get_handle_entity },

	{ "ActionResult.GetReason",                       actions_natives::native_action_result_get_reason },
	{ "ActionResult.SetReason",                       actions_natives::native_action_result_set_reason },

	{ "ActionResult.type.set",                        actions_natives::native_action_result_set_type },
	{ "ActionResult.type.get",                        actions_natives::native_action_result_get_type },

	{ "ActionResult.action.set",                      actions_natives::native_action_result_set_action },
	{ "ActionResult.action.get",                      actions_natives::native_action_result_get_action },

	{ "ActionDesiredResult.priority.set",             actions_natives::native_desired_result_set_priority },
	{ "ActionDesiredResult.priority.get",             actions_natives::native_desired_result_get_priority },

	{ nullptr, nullptr }
};

sp_nativeinfo_t g_actionsNativesLegacy[] =
{
	{ "ActionsManager.Allocate",    actions_natives::native_legacy_allocate },
	{ "ActionsManager.Deallocate",  actions_natives::native_legacy_deallocate },
	{ "ActionsManager.Iterator",    actions_natives::native_legacy_iterate_entity_actions },
	{ "ActionsManager.GetAction",   actions_natives::native_legacy_find_entity_action },
	{ "ActionsManager.Create",      actions_natives::native_legacy_create_action },

	{ nullptr, nullptr }
};

sp_nativeinfo_t g_actionsTransitionNatives[] =
{
	{ "ActionTransitionContext.Type.get",    			actions_natives::native_transition_context_get_kind },
	{ "ActionTransitionContext.GetFloat",    			actions_natives::native_transition_context_get_float },
	{ "ActionTransitionContext.GetInt",    				actions_natives::native_transition_context_get_int },
	{ "ActionTransitionContext.GetBool",    			actions_natives::native_transition_context_get_bool },
	{ "ActionTransitionContext.GetPathFollower",    	actions_natives::native_transition_context_get_path_follower },
	{ "ActionTransitionContext.GetKnownEntity",    		actions_natives::native_transition_context_get_known_entity },
	{ "ActionTransitionContext.GetVector",    			actions_natives::native_transition_context_get_vector },
	{ "ActionTransitionContext.GetActionResult",    	actions_natives::native_transition_context_get_action_result },
	{ "ActionTransitionContext.GetEventDesiredResult",  actions_natives::native_transition_context_get_desired_result },
	{ "ActionTransitionContext.GetAction",  			actions_natives::native_transition_context_get_action_ptr },
	{ "ActionTransitionContext.GetQueryResult",  		actions_natives::native_transition_context_get_query_result },
	{ "ActionTransitionContext.GetFunctionName",  		actions_natives::native_transition_context_get_function_name },

	{ nullptr, nullptr }
};


#ifdef INCLUDE_ACTIONS_CONSTRUCTOR

#include "actions/natives/constructor.h"

sp_nativeinfo_t g_actionsNativesConstructor[] =
{
	{ "ActionConstructor.ActionConstructor",  actions_natives::native_constructor_create },
	{ "ActionConstructor.SetupFromConf",      actions_natives::native_constructor_setup_from_config },
	{ "ActionConstructor.AddressFromConf",    actions_natives::native_constructor_address_from_config },
	{ "ActionConstructor.SignatureFromConf",  actions_natives::native_constructor_signature_from_config },
	{ "ActionConstructor.AddParameter",       actions_natives::native_constructor_add_parameter },
	{ "ActionConstructor.Finish",             actions_natives::native_constructor_finish },
	{ "ActionConstructor.Execute",            actions_natives::native_constructor_execute },
	{ "ActionConstructor.Address",            actions_natives::native_constructor_address },
	{ "ActionConstructor.Size",               actions_natives::native_constructor_size },
	// { "ActionConstructor.Convention",       actions_natives::native_constructor_convention },

	{ nullptr, nullptr }
};
#else
sp_nativeinfo_t g_actionsNativesConstructor[] =
{
	{ nullptr, nullptr }
};
#endif // INCLUDE_ACTIONS_CONSTRUCTOR

#endif // ACTIONS_NATIVES_H
