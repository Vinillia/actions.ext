#ifndef _INCLUDE_ACTIONS_PROCESSOR_SHARED_H
#define _INCLUDE_ACTIONS_PROCESSOR_SHARED_H

#include "NextBotBehavior.h"

#undef clamp // **** ***

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
struct execution_result
{
	hexecution<T> handlerExecution;
	T effectiveResult;

	const ResultType& rt() {	
		return handlerExecution.rt; 
	}
	
	const T& rv()	{	
		return handlerExecution.rv; 
	}

	const T& result() {
		return effectiveResult;
	}
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

template<typename R, typename M, typename ...Args>
hexecution<R> ProcessHandlerImpl(M&& handler, nb_action_ptr action, HashValue hash, R& result, Args&&... args)
{
	ResultType plpre = Pl_Continue, plpost = Pl_Continue;
	hexecution<R> he = {};

	g_actionsManager.PushRuntimeResult(&result);
	plpre = g_actionsPropagationPre.ProcessMethod<R, Args...>(action, hash, &result, std::forward<Args>(args)...);

	if (plpre != Pl_Handled)
	{
		he.rv = handler();

		if constexpr (is_action_result_v<R>)
		{
			if (he.rv.m_action)
				g_actionsManager.AddPending(he.rv.m_action);
		}

		if (plpre < Pl_Changed)
			result = he.rv;
	}

	plpost = g_actionsPropagationPost.ProcessMethod<R, Args...>(action, hash, &result, std::forward<Args>(args)...);
	g_actionsManager.PopRuntimeResult();

	if (plpre < Pl_Changed && plpost < Pl_Changed)
		result = he.rv;

	if constexpr (is_action_result_v<R>)
	{
		if (result.m_action == nullptr && (result.m_type == CHANGE_TO || result.m_type == SUSPEND_FOR))
			result = he.rv;

		g_actionsManager.ProcessResult(action, reinterpret_cast<const ActionResult<CBaseEntity>&>(result));
	}

	he.rt = std::max<decltype(he.rt)>(plpre, plpost);
	return he;
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
	using return_t = decltype(((action)->*handler)(args...));
	
	// Error C2131 expression did not evaluate to a constant
	// constexpr bool isVoid = std::is_void_v<return_t>; 

	auto ul = [&]() -> return_t
	{
		if constexpr (!std::is_void_v<return_t>)
		{
			if constexpr (!is_contextual_query<M>::value)
			{
				if constexpr (std::is_same_v<return_t, Action< CBaseEntity >*>)
				{
					const return_t child = std::invoke(handler, (A)action, std::forward<Args>(args)...);

					if (child != nullptr)
						g_actionsManager.Add(child);

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

	if constexpr (!std::is_void_v<return_t>)
	{
		execution_result<return_t> er = {};
		er.handlerExecution = ProcessHandlerImpl(ul, reinterpret_cast<nb_action_ptr>(action), hash, er.effectiveResult, std::forward<Args>(args)...);
		return er;
	}
	else
	{
		return ProcessHandlerImplVoid(ul, reinterpret_cast<nb_action_ptr>(action), hash, std::forward<Args>(args)...);
	}
}

template<typename M, typename A, typename ...Args>
inline decltype(auto) ProcessHandler(HashValue hash, A action, M&& handler, Args&& ...args)
{
	return ProcessHandlerEx(hash, action, std::forward<M>(handler), std::forward<Args>(args)...).effectiveResult;
}

#endif // !_INCLUDE_ACTIONS_PROCESSOR_SHARED_H
