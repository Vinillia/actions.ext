#pragma once

#ifndef _INCLUDE_ACTIONS_PROPAGATE_H
#define _INCLUDE_ACTIONS_PROPAGATE_H

#include "utils.h"

#include "extension.h"
#include "actions_manager.h"

#include <vector>

#include <am-hashset.h>
#include <am-hashmap.h>
#include <am-fixedarray.h>

#include "NextBotBehavior.h"

class ActionsPropagate
{
	friend class ActionsManager;

	static const cell_t SIZE = 100;

	using Action = ActionsManager::Action;

	using PluginCallbacks = std::vector<IPluginFunction*>;
	using HandlersArray = ke::FixedArray<PluginCallbacks>;
	using ActionsHandler = ke::HashMap<Action*, HandlersArray, ke::PointerPolicy<Action>>;

	static void OnActionAdded(Action* action);
	static void OnActionDestroyed(Action* action);

public:
	ActionsPropagate();
	~ActionsPropagate() = delete;

	void Dump();

	bool AddListener(int32_t vtableidx, Action* action, IPluginFunction* listener);
	
	bool RemoveListener(int32_t vtableidx, Action* action, IPluginFunction* listener);
	bool RemoveListener(int32_t vtableidx, Action* action, IPluginContext* context);

	void RemoveListeners(Action* action, IPluginContext* context);
	void RemoveListeners(IPluginContext* context);
	void RemoveListeners(Action* action);

	bool FindListener(int32_t vtableidx, Action* action, IPluginFunction* listener, PluginCallbacks::iterator* iter = NULL);
	bool FindListener(int32_t vtableidx, IPluginFunction* listener, PluginCallbacks::iterator* iter = NULL);

	template<typename T>
	void ProcessHandleArg(PluginCallbacks& callbacks, T&& arg)
	{
		using type = std::remove_const_t<std::remove_reference_t<T>>;

		for (auto listener : callbacks)
		{
			if constexpr (std::is_same<type, float>::value)
			{
				listener->PushFloat((float)arg);
			}
			else if constexpr (std::is_same<type, char*>::value)
			{
				listener->PushString((char*)arg);
			}
			else if constexpr (std::is_same<type, Vector>::value)
			{
				listener->PushArray((cell_t*)&arg, sizeof(Vector));
			}
			else if constexpr (std::is_same<type, CBaseEntity*>::value)
			{
				cell_t entity = -1;

				if (arg != NULL)
					entity = gamehelpers->EntityToBCompatRef(arg);

				listener->PushCell(entity);
			}
			else if constexpr (std::is_same<type, int>::value || std::is_pointer<T>::value || std::is_enum<T>::value)
			{
				listener->PushCell((cell_t)arg);
			}
			else
			{
				/* TO DO: */ 
				/* EDIT: TO DO WHAT? */
				listener->PushCell((cell_t)arg);
				//typedef typename T::something_made_up X;
				//bool x = decltype(arg)::nothing; 
				//static_assert(false);
			}
		}
	}

	template<typename returnType, typename ...Args>
	ResultType ProcessHandler(int32_t vtableidx, Action* action, returnType* result, Args&&... args)
	{
		constexpr int32_t num = sizeof...(Args);

		auto r = m_handlers.find(action);
		
		if (!r.found())
			return Pl_Continue;

		auto& listeners = r->value[vtableidx];

		if (listeners.size() == 0)
			return Pl_Continue;

		ResultType returnResult, executeResult = Pl_Continue;
		returnResult = executeResult;

		g_pActionsManager->SetRuntimeResult((void*)result);
		ProcessHandleArg<Action*>(listeners, std::forward<Action*>(action));
		(ProcessHandleArg<Args>(listeners, std::forward<Args>(args)), ...);

		for (IPluginFunction* listener : listeners)
		{
			if constexpr (std::is_same<returnType, ActionResult<void>>::value || std::is_same<returnType, EventDesiredResult<void>>::value)
			{
				listener->PushCell((cell_t)result);
			}
			else
			{
				listener->PushCellByRef((cell_t*)result);
			}

			listener->Execute((cell_t*)&executeResult);

			if constexpr (std::is_same<returnType, ActionResult<void>>::value || std::is_same<returnType, EventDesiredResult<void>>::value)
			{
				if (result->IsRequestingChange() && !result->IsDone())
				{
					if (result->m_action == nullptr)
					{
						listener->GetParentRuntime()->GetDefaultContext()->ReportError("Tried to change to NULL action");
						break;
					}

					Action* runtimeAction = g_pActionsManager->GetRuntimeAction();

					if (runtimeAction && result->m_action != runtimeAction)
					{
						g_pActionsManager->SetRuntimeAction(result->m_action);
						delete runtimeAction;
					}
				}
			}

			if (executeResult > returnResult)
				returnResult = executeResult;
		}

		return returnResult;
	}

private:
	ActionsHandler m_handlers;
	bool m_init;
};

extern ActionsPropagate* g_pActionsPropagatePre;
extern ActionsPropagate* g_pActionsPropagatePost;

#endif // _INCLUDE_ACTIONS_PROPAGATE_H
