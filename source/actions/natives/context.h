#ifndef ACTIONS_NATIVE_CONTEXT_H
#define ACTIONS_NATIVE_CONTEXT_H

#include <cstdarg>
#include <exception>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace actions_natives
{
	class format_exception final : public std::exception
	{
	public:
		explicit format_exception(const char* fmt, ...)
		{
			char buffer[1024];

			va_list ap;
			va_start(ap, fmt);
			g_pSM->FormatArgs(buffer, sizeof(buffer), fmt, ap);
			va_end(ap);

			_message = buffer;
		}

		const char* what() const noexcept override
		{
			return _message.c_str();
		}

	private:
		std::string _message;
	};

	struct native_abort_exception final
	{
	};

	template<typename T>
	static inline T native_default_return()
	{
		if constexpr (std::is_pointer_v<T>)
			return nullptr;
		else
			return T{};
	}

	template<
		typename T,
		typename... Extractors,
		typename Callback>
	static inline T execute_native_context(
		IPluginContext* context,
		const cell_t*,
		std::index_sequence<>,
		std::tuple<Extractors...>&,
		Callback&& callback)
	{
		try
		{
			return std::forward<Callback>(callback)();
		}
		catch (const native_abort_exception&)
		{
			return native_default_return<T>();
		}
		catch (const std::exception& exception)
		{
			context->ReportError("%s", exception.what());
			return native_default_return<T>();
		}
	}

	template<
		typename T,
		size_t... Is,
		typename... Extractors,
		typename Callback>
	static inline T execute_native_context(
		IPluginContext* context,
		const cell_t* params,
		std::index_sequence<Is...>,
		std::tuple<Extractors...>& extractors,
		Callback&& callback)
	{
		try
		{
			auto args = std::tuple<
				decltype(std::get<Is>(extractors)(context, params[Is + 1], static_cast<cell_t>(Is + 1)))...>
		{
				std::get<Is>(extractors)(context, params[Is + 1], static_cast<cell_t>(Is + 1))...
			};

			return std::apply(std::forward<Callback>(callback), std::move(args));
		}
		catch (const native_abort_exception&)
		{
			return native_default_return<T>();
		}
		catch (const std::exception& exception)
		{
			context->ReportError("%s", exception.what());
			return native_default_return<T>();
		}
	}

	template<typename T, typename Callback, typename... Extractors>
	static inline T execute_native_context(
		IPluginContext* context,
		const cell_t* params,
		Callback&& callback,
		Extractors... extractors)
	{
		auto extractor_tuple = std::make_tuple(extractors...);
		constexpr size_t count = sizeof...(Extractors);

		return execute_native_context<T>(
			context,
			params,
			std::make_index_sequence<count>{},
			extractor_tuple,
			std::forward<Callback>(callback));
	}

	static inline cell_t native_cell_extractor(IPluginContext*, cell_t param, cell_t)
	{
		return param;
	}

	static inline HashValue native_hash_extractor(IPluginContext*, cell_t param, cell_t)
	{
		return static_cast<HashValue>(param);
	}

	static inline bool native_bool_extractor(IPluginContext*, cell_t param, cell_t)
	{
		return param != 0;
	}

	static inline float native_float_extractor(IPluginContext*, cell_t param, cell_t)
	{
		return sp_ctof(param);
	}

	static inline cell_t* native_cell_ptr_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		cell_t* value = nullptr;
		context->LocalToPhysAddr(param, &value);
		return value;
	}

	static inline const char* native_string_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		char* value = nullptr;
		context->LocalToString(param, &value);
		return value;
	}

	static inline char* native_mutable_string_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		char* value = nullptr;
		context->LocalToString(param, &value);
		return value;
	}

	static inline const char* native_nullable_string_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		char* value = nullptr;
		context->LocalToStringNULL(param, &value);
		return value;
	}

	static inline Vector native_vector_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		cell_t* vec = nullptr;
		context->LocalToPhysAddr(param, &vec);
		return Vector(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
	}

	static inline Vector* native_vector_ptr_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		cell_t* vec = nullptr;
		context->LocalToPhysAddr(param, &vec);

		if (context->GetNullRef(SP_NULL_VECTOR) == vec)
			return nullptr;

		return reinterpret_cast<Vector*>(vec);
	}

	static inline QAngle native_angle_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		cell_t* vec = nullptr;
		context->LocalToPhysAddr(param, &vec);
		return QAngle(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
	}

	static inline QAngle* native_angle_ptr_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		cell_t* vec = nullptr;
		context->LocalToPhysAddr(param, &vec);

		if (context->GetNullRef(SP_NULL_VECTOR) == vec)
			return nullptr;

		return reinterpret_cast<QAngle*>(vec);
	}

	static inline CBaseEntity* native_entity_extractor(IPluginContext*, cell_t param, cell_t num)
	{
		CBaseEntity* entity = gamehelpers->ReferenceToEntity(param);
		if (!entity)
			throw format_exception("invalid entity index at %i (%i)", num, param);

		return entity;
	}

	static inline CBaseEntity* native_nullable_entity_extractor(IPluginContext*, cell_t param, cell_t)
	{
		return gamehelpers->ReferenceToEntity(param);
	}

	static inline CBaseEntity* native_nextbot_entity_extractor(IPluginContext*, cell_t param, cell_t num)
	{
		CBaseEntity* entity = gamehelpers->ReferenceToEntity(param);
		if (!entity)
			throw format_exception("invalid entity index at %i (%i)", num, param);

		if (!g_pActionsTools->MyNextBotPointer(entity))
			throw format_exception("invalid nextbot entity at %i (%i)", num, param);

		return entity;
	}

	static inline IPluginFunction* native_function_extractor(IPluginContext* context, cell_t param, cell_t num)
	{
		IPluginFunction* function = context->GetFunctionById(param);
		if (!function)
			throw format_exception("invalid function index at %i (%i)", num, param);

		return function;
	}

	static inline IPluginFunction* native_nullable_function_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		return context->GetFunctionById(param);
	}

	static inline void* native_address_extractor(
		IPluginContext*,
		cell_t param,
		cell_t num)
	{
		void* address = FromPseudoAddress<void*>(param);

		if (reinterpret_cast<uintptr_t>(address) <= 0x1000)
			throw format_exception("invalid address at %i (%X)", num, param);

		return address;
	}

	static inline nb_action_ptr native_action_extractor(IPluginContext*, cell_t param, cell_t num)
	{
		nb_action_ptr action = FromPseudoAddress<nb_action_ptr>(param);

		if (!g_actionsManager.IsValidAction(action))
			throw format_exception("invalid action at %i (%X)", num, param);

		return action;
	}

	static inline ActionResult<CBaseEntity>* native_action_result_extractor(IPluginContext* ctx, cell_t param, cell_t num)
	{
		ActionResult<CBaseEntity>* runtime_result = g_actionsManager.GetActionRuntimeResult();

		if (!runtime_result)
		{
			throw format_exception("Attempt to access an invalid result.");
		}

		if (runtime_result != native_address_extractor(ctx, param, num))
		{
			throw format_exception(
				"Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!",
				runtime_result,
				native_address_extractor(ctx, param, num));
		}

		return runtime_result;
	}

	static inline EventDesiredResult<CBaseEntity>* native_action_desired_result_extractor(IPluginContext* ctx, cell_t param, cell_t num)
	{
		EventDesiredResult<CBaseEntity>* runtime_result = g_actionsManager.GetActionRuntimeDesiredResult();

		if (!runtime_result)
		{
			throw format_exception("Attempt to access an invalid result.");
		}

		if (runtime_result != native_address_extractor(ctx, param, num))
		{
			throw format_exception(
				"Attempt to access an invalid result (%X != %X). Make sure function callback matches prototype!",
				runtime_result,
				native_address_extractor(ctx, param, num));
		}

		return runtime_result;
	}

	static inline ActionComponent* read_component_handle(
		IPluginContext* context,
		cell_t param,
		bool require_owner)
	{
		const Handle_t handle = static_cast<Handle_t>(param);
		HandleError error;
		ActionComponent* component = nullptr;

		HandleSecurity security;
		security.pOwner = require_owner ? context->GetIdentity() : nullptr;
		security.pIdentity = myself->GetIdentity();

		error = g_pHandleSys->ReadHandle(
			handle,
			g_sdkActions.GetComponentHT(),
			&security,
			reinterpret_cast<void**>(&component));

		if (error != HandleError_None)
		{
			context->ThrowNativeError("Invalid component handle %x (error %d)", handle, error);
			throw native_abort_exception{};
		}

		return component;
	}

	static inline ActionComponent* native_component_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		return read_component_handle(context, param, false);
	}

	static inline ActionComponent* native_owned_component_extractor(IPluginContext* context, cell_t param, cell_t)
	{
		return read_component_handle(context, param, true);
	}
}

#endif // ACTIONS_NATIVE_CONTEXT_H
