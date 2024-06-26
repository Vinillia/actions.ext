#ifndef _INCLUDE_ACTIONS_PROCESSOR_SHARED_H
#define _INCLUDE_ACTIONS_PROCESSOR_SHARED_H

#include "NextBotBehavior.h"

#ifdef clamp
#undef clamp
#endif

#include "actions_propagation.h"
#include "actions_pubvars.h"
#include "actionsdefs.h"

#include "actions_processor_impl.h"

template <typename... Ts>
using void_t = void;

template <class K, typename T, typename = void>
struct is_class_member : std::false_type {};

template <class K, typename T>
struct is_class_member<K, T, void_t<typename std::enable_if_t<std::is_same_v<T, void(K::*)()>>>> : std::true_type {};

template <typename T>
struct is_contextual_query : is_class_member<IContextualQuery, T> {};

template<typename T>
inline static void _construct_type(T& value)
{
	if constexpr (std::is_same_v<T, Vector>)
	{
		value.x = 0.0f; value.y = 0.0f; value.z = 0.0f;
	}
	else if constexpr  (std::is_pointer_v<T>)
	{
		value = nullptr;
	}
	else
	{
		value = T();
	}
}

template<typename T>
struct Handler
{
	Handler()
	{
		result_type = Pl_Continue;
		_construct_type(result);
	}

	ResultType result_type;
	T result;
};

template<typename T>
struct Execution
{
	Execution()
	{
		_construct_type(result);
	}

	Handler<T> handler;
	T result;
};

class ActionProcessorShared : public Action<CBaseEntity>
{
public:
	ActionProcessorShared();

private:
	virtual INextBotEventResponder* FirstContainedResponder(void) const override;
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const override;

protected:
	virtual const char* GetName(void) const override;
	virtual bool IsNamed(const char* name) const override;
	virtual const char* GetFullName(void) const override;

public:
	virtual ActionResult< CBaseEntity >	OnStart(CBaseEntity* me, Action< CBaseEntity >* priorAction) override;
	virtual ActionResult< CBaseEntity >	Update(CBaseEntity* me, float interval) override;
	virtual void						OnEnd(CBaseEntity* me, Action< CBaseEntity >* nextAction) override;
	virtual ActionResult< CBaseEntity >	OnSuspend(CBaseEntity* me, Action< CBaseEntity >* interruptingAction) override;
	virtual ActionResult< CBaseEntity >	OnResume(CBaseEntity* me, Action< CBaseEntity >* interruptingAction) override;
	virtual Action< CBaseEntity >* InitialContainedAction(CBaseEntity* me) override;
};

extern ActionProcessorShared* g_pActionProcessor;

template<typename T>
static void ProcessResultTransition(T& desiredResult)
{
	// if listener required to change action
	if (desiredResult.m_type == CHANGE_TO || desiredResult.m_type == SUSPEND_FOR)
	{
		// desired action is null, report it.
		if (!desiredResult.m_action)
		{
			g_pSM->LogError(myself, "Trying to change to null action!");
			desiredResult.m_type = CONTINUE;
		}
		else
		{
			// Transition has been successful but new action isn't final yet.
			g_actionsManager.AddPending(desiredResult.m_action);
		}
	}
	else if (desiredResult.m_action)
	{
		g_pSM->LogError(myself, "Trying to change action without request!");
	}
}

template<typename TReturn, typename TMethod, typename ...Args>
Handler<TReturn> ProcessHandlerImpl(TMethod&& method, nb_action_ptr action, HashValue hash, TReturn& newResult, Args&&... args)
{
	ResultType plpre = Pl_Continue, plpost = Pl_Continue;
	Handler<TReturn> handler;

	g_actionsManager.PushRuntimeResult(&newResult);
	plpre = g_actionsPropagationPre.ProcessMethod<TReturn, Args...>(action, hash, &newResult, std::forward<Args>(args)...);

	if (plpre != Pl_Handled)
	{
		// Call original handler if hasn't been blocked
		handler.result = method();

		if (plpre < Pl_Changed)
		{
			// No answer from listeners. Keep original return value
			newResult = handler.result;
		}
	}

	if constexpr (is_action_result_v<TReturn>)
	{
		g_actionsManager.AddPending(newResult.m_action);
	}

	plpost = g_actionsPropagationPost.ProcessMethod<TReturn, Args...>(action, hash, &newResult, std::forward<Args>(args)...);
	g_actionsManager.PopRuntimeResult();

	// Either pre or post listeners changed result. Use handler's return
	// if (plpre < Pl_Changed && plpost < Pl_Changed)
	//		newResult = handler.value;

	if constexpr (is_action_result_v<TReturn>)
	{
		ProcessResultTransition(newResult);
		g_actionsManager.ProcessResult(action, reinterpret_cast<const ActionResult<CBaseEntity>&>(newResult));
	}

	handler.result_type = std::max<ResultType>(plpre, plpost);
	return handler;
}

template<typename M, typename ...Args>
ResultType ProcessHandlerImplVoid(M&& method, nb_action_ptr action, HashValue hash, Args&&... args)
{
	ResultType plpre = Pl_Continue, plpost = Pl_Continue;

	plpre = g_actionsPropagationPre.ProcessMethod<std::nullptr_t, Args...>(action, hash, nullptr, std::forward<Args>(args)...);

	if (plpre != Pl_Handled)
		method();

	plpost = g_actionsPropagationPost.ProcessMethod<std::nullptr_t, Args...>(action, hash, nullptr, std::forward<Args>(args)...);
	return std::max<ResultType>(plpre, plpost);
}

template<typename M, typename A, typename ...Args>
inline decltype(auto) ProcessHandlerEx(HashValue hash, A action, M&& handler, Args&& ...args)
{
	Autoswap guard(action);
	using TReturn = decltype(((action)->*handler)(args...));

	// Error C2131 expression did not evaluate to a constant
	// constexpr bool isVoid = std::is_void_v<TReturn>; 

	auto ul = [&]() -> TReturn
		{
			if constexpr (!std::is_void_v<TReturn>)
			{
				if constexpr (!is_contextual_query<M>::value)
				{
					if constexpr (std::is_same_v<TReturn, Action< CBaseEntity >*>)
					{
						const TReturn child = std::invoke(handler, (A)action, std::forward<Args>(args)...);

						if (child != nullptr)
						{
							g_actionsManager.SetActionActor(child, action->GetActor());
							g_actionsManager.Add(child);
						}

						return child;
					}
					else
					{
						return std::invoke(handler, (A)action, std::forward<Args>(args)...);
					}
				}
				else
				{
					return std::invoke(handler, static_cast<IContextualQuery*>(action), std::forward<Args>(args)...);
				}
			}
			else
			{
				if constexpr (!is_contextual_query<M>::value)
				{
					std::invoke(handler, (A)action, std::forward<Args>(args)...);
				}
				else
				{
					std::invoke(handler, static_cast<IContextualQuery*>(action), std::forward<Args>(args)...);
				}
			}
		};

	if constexpr (!std::is_void_v<TReturn>)
	{
		Execution<TReturn> execution = {};
		execution.handler = ProcessHandlerImpl(ul, reinterpret_cast<nb_action_ptr>(action), hash, execution.result, std::forward<Args>(args)...);
		return execution;
	}
	else
	{
		return ProcessHandlerImplVoid(ul, reinterpret_cast<nb_action_ptr>(action), hash, std::forward<Args>(args)...);
	}
}

template<typename M, typename A, typename ...Args>
inline decltype(auto) ProcessHandler(HashValue hash, A action, M&& handler, Args&& ...args)
{
	return ProcessHandlerEx(hash, action, std::forward<M>(handler), std::forward<Args>(args)...).result;
}

#endif // !_INCLUDE_ACTIONS_PROCESSOR_SHARED_H
