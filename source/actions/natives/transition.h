#ifndef ACTIONS_NATIVE_TRANSITION_H
#define ACTIONS_NATIVE_TRANSITION_H

#include "actions/natives/context.h"

namespace actions_natives
{
	static TransitionContext* native_transition_context_extractor(IPluginContext*,
		cell_t param,
		cell_t num)
	{
		HandleSecurity security;

		security.pIdentity = myself->GetIdentity();
		security.pOwner = myself->GetIdentity();

		TransitionContext* context = nullptr;

		HandleError error = g_pHandleSys->ReadHandle(static_cast<Handle_t>(param), g_sdkActions.GetTransitionContextHT(), &security, reinterpret_cast<void**>(&context));

		if (error != HandleError_None)
		{
			throw format_exception("invalid transition context handle at %i (%x, error %d)", num, param, error);
		}

		return context;
	}

	template<typename T>
	static T native_transition_context_get_plain_data(const TransitionContext* context)
	{
		if (sizeof(T) != context->size)
			throw format_exception("invalid transition context data type");

		return *context->As<T>();
	}

	static void validate_context_kind(const TransitionContext* context, TransitionContext::Kind kind)
	{
		if (context->kind != kind)
		{
			throw format_exception("invalid transition context kind (%d != %d)", context->kind, kind);
		}
	}

	static inline cell_t native_transition_context_get_kind(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				return static_cast<cell_t>(context->kind);
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_float(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::PlainData);

				float value = native_transition_context_get_plain_data<float>(context);
				return sp_ftoc(value);
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_int(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::PlainData);
				
				return native_transition_context_get_plain_data<cell_t>(context);
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_bool(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::PlainData);

				return static_cast<cell_t>(native_transition_context_get_plain_data<bool>(context));
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_query_result(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::QueryResult);

				return static_cast<cell_t>(*context->As<QueryResultType>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_action_ptr(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::ActionPtr);

				return ToPseudoAddress(*context->As<nb_action_ptr>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_action_result(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::ActionResult);

				return ToPseudoAddress(context->As<ActionResult<CBaseEntity>>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_desired_result(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::DesiredResult);

				return ToPseudoAddress(context->As<EventDesiredResult<CBaseEntity>>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_vector(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context, Vector* out) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::Vector);

				*out = *context->As<Vector>();
				return 0;
			},
			native_transition_context_extractor,
			native_vector_ptr_extractor);
	}

	static inline cell_t native_transition_context_get_path_follower(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::PathFollower);
				return ToPseudoAddress(*context->As<PathFollower*>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_known_entity(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](TransitionContext* context) -> cell_t
			{
				validate_context_kind(context, TransitionContext::Kind::KnownEntity);
				return ToPseudoAddress(*context->As<CKnownEntity*>());
			},
			native_transition_context_extractor);
	}

	static inline cell_t native_transition_context_get_function_name(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](TransitionContext* transitionContext, cell_t outName, cell_t length) -> cell_t
			{
				std::string name = g_publicsManager.GetName(transitionContext->hash);
				
				/* remove prefix */
				/* __action_processor_OnUpdate -> OnUpdate */
				name.erase(0, ActionPublicsManager::pubvar_prefix.size());

				return context->StringToLocal(outName, length, name.c_str());
			},
			native_transition_context_extractor,
			native_cell_extractor,
			native_cell_extractor);
	}
}

#endif // ACTIONS_NATIVE_TRANSITION_H
