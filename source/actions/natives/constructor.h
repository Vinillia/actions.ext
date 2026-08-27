#ifndef ACTIONS_NATIVE_CONSTRUCTOR_H
#define ACTIONS_NATIVE_CONSTRUCTOR_H

#include "actions/natives/context.h"

#include <memory>

namespace actions_natives
{
	static inline ActionConstructor* read_constructor_handle(
		IPluginContext* context,
		cell_t param)
	{
		const Handle_t handle = static_cast<Handle_t>(param);
		HandleSecurity security(nullptr, myself->GetIdentity());
		HandleError error;
		ActionConstructor* constructor = nullptr;

		error = g_pHandleSys->ReadHandle(
			handle,
			g_sdkActions.GetConstructorHT(),
			&security,
			reinterpret_cast<void**>(&constructor));

		if (error != HandleError_None)
			throw format_exception("invalid constructor handle %x (error %d)", handle, error);

		return constructor;
	}

	static inline ActionConstructor* native_constructor_extractor(
		IPluginContext* context,
		cell_t param,
		cell_t)
	{
		return read_constructor_handle(context, param);
	}

	static inline IGameConfig* native_game_config_extractor(
		IPluginContext* context,
		cell_t param,
		cell_t num)
	{
		HandleError error;
		IGameConfig* config = gameconfs->ReadHandle(param, context->GetIdentity(), &error);

		if (!config)
			throw format_exception("invalid gamedata handle at %i (%x, error %d)", num, param, error);

		return config;
	}

	static inline ActionEncoder* native_action_encoder_extractor(
		IPluginContext*,
		cell_t param,
		cell_t)
	{
		return FromPseudoAddress<ActionEncoder*>(param);
	}

	static inline cell_t create_constructor_handle(
		IPluginContext* context,
		std::unique_ptr<ActionConstructor> constructor)
	{
		HandleError error;
		const Handle_t handle = handlesys->CreateHandle(
			g_sdkActions.GetConstructorHT(),
			constructor.get(),
			context->GetIdentity(),
			myself->GetIdentity(),
			&error);

		if (handle == BAD_HANDLE)
			throw format_exception("failed to create constructor handle (error %d)", error);

		constructor.release();
		return static_cast<cell_t>(handle);
	}

	static inline cell_t native_constructor_create(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](cell_t value) -> cell_t
			{
				return create_constructor_handle(
					context,
					std::make_unique<ActionConstructor>(value));
			},
			native_cell_extractor);
	}

	static inline cell_t native_constructor_setup_from_config(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](IGameConfig* config, const char* key) -> cell_t
			{
				auto* data = g_pActionConstructorSMC->GetACData(key);
				if (!data)
					throw format_exception("invalid action constructor (%s)", key);

				auto constructor = std::make_unique<ActionConstructor>();
				if (!constructor->SetupFromConf(context, config, data))
					return 0;

				return create_constructor_handle(context, std::move(constructor));
			},
			native_game_config_extractor,
			native_string_extractor);
	}

	static inline cell_t native_constructor_address_from_config(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionConstructor* constructor, IGameConfig* config, const char* key) -> cell_t
			{
				return constructor->AddressFromConf(context, config, key);
			},
			native_constructor_extractor,
			native_game_config_extractor,
			native_string_extractor);
	}

	static inline cell_t native_constructor_signature_from_config(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context](ActionConstructor* constructor, IGameConfig* config, const char* key) -> cell_t
			{
				return constructor->SignatureFromConf(context, config, key);
			},
			native_constructor_extractor,
			native_game_config_extractor,
			native_string_extractor);
	}

	static inline cell_t native_constructor_add_parameter(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionConstructor* constructor, cell_t pass_type, cell_t flags, ActionEncoder* encoder) -> cell_t
			{
				return static_cast<cell_t>(constructor->AddParameter(
					static_cast<PassType>(pass_type),
					flags,
					encoder));
			},
			native_constructor_extractor,
			native_cell_extractor,
			native_cell_extractor,
			native_action_encoder_extractor);
	}

	static inline cell_t native_constructor_finish(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionConstructor* constructor) -> cell_t
			{
				return static_cast<cell_t>(constructor->Finish());
			},
			native_constructor_extractor);
	}

	static inline cell_t native_constructor_execute(IPluginContext* context, const cell_t* params)
	{
		return execute_native_context<cell_t>(
			context,
			params,
			[context, params](ActionConstructor* constructor) -> cell_t
			{
				nb_action_ptr action = nullptr;

				try
				{
					const cell_t count = params[0] - 1;
					action = constructor->Execute(context, params, count);
				}
				catch (const std::exception& exception)
				{
					throw format_exception("failed to execute constructor: %s", exception.what());
				}

				if (action)
				{
					g_actionsManager.SetActionActor(action, nullptr);
					g_actionsManager.AddPending(action);

					BeginActionProcessing(action);
				}

				return ToPseudoAddress(action);
			},
			native_constructor_extractor);
	}

	static inline cell_t native_constructor_address(IPluginContext* context, const cell_t* params)
	{
		if (params[0] > 1)
		{
			return execute_native_context<cell_t>(
				context,
				params,
				[](ActionConstructor* constructor, void* address) -> cell_t
				{
					constructor->SetAddress(address);
					return ToPseudoAddress(constructor->GetAddress());
				},
				native_constructor_extractor,
				native_address_extractor);
		}

		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionConstructor* constructor) -> cell_t
			{
				return ToPseudoAddress(constructor->GetAddress());
			},
			native_constructor_extractor);
	}

	static inline cell_t native_constructor_size(IPluginContext* context, const cell_t* params)
	{
		if (params[0] > 1)
		{
			return execute_native_context<cell_t>(
				context,
				params,
				[](ActionConstructor* constructor, cell_t size) -> cell_t
				{
					constructor->SetSize(size);
					return static_cast<cell_t>(constructor->GetSize());
				},
				native_constructor_extractor,
				native_cell_extractor);
		}

		return execute_native_context<cell_t>(
			context,
			params,
			[](ActionConstructor* constructor) -> cell_t
			{
				return static_cast<cell_t>(constructor->GetSize());
			},
			native_constructor_extractor);
	}
}

#endif // ACTIONS_NATIVE_CONSTRUCTOR_H
