#include "actions_propagation.h"

#include <amtl/am-vector.h>
#include <vector>
#include <optional>

#include "extension.h"

ActionPropagation g_actionsPropagationPre;
ActionPropagation g_actionsPropagationPost;

ActionPropagation::ActionPropagation()
{
}

ActionPropagation::~ActionPropagation()
{
}

bool ActionPropagation::AddListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	if (!g_actionsManager.IsValidAction(action))
	{
		fn->GetParentRuntime()->GetDefaultContext()->ReportError("Attempt to hook invalid action");
		return false;
	}

	auto& v = m_actionsListeners[action][hash];
	auto listener = std::find_if(v.cbegin(), v.cend(), [fn](const ActionListener& listener) { return listener.fn == fn; });

	if (listener != v.end())
		return false;

	m_actionsListeners[action][hash].emplace_back(hash, fn);
	return true;
}

bool ActionPropagation::RemoveListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	if (!g_actionsManager.IsValidAction(action))
	{
		fn->GetParentRuntime()->GetDefaultContext()->ReportError("Attempt to hook invalid action");
		return false;
	}

	auto& v = m_actionsListeners[action][hash];
	auto listener = std::find_if(v.cbegin(), v.cend(), [fn](const ActionListener& listener) { return listener.fn == fn; });

	if (listener == v.end())
		return false;

	v.erase(listener);
	return true;
}

bool ActionPropagation::RemoveListener(nb_action_ptr action, HashValue hash, IPluginContext* ctx)
{
	auto& v = m_actionsListeners[action][hash];
	auto result = std::find_if(v.begin(), v.end(), [ctx](const ActionListener& listener)
		{ 
			return ctx == listener.fn->GetParentRuntime()->GetDefaultContext(); 
		});
	
	if (result == v.end())
		return false;

	v.erase(result);
	return true;
}

ActionListener* ActionPropagation::HasListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	auto& list = m_actionsListeners[action][hash];

	for (auto iter = list.begin(); iter != list.end(); iter++)
	{
		if (iter->fn == fn && iter->hash == hash)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

void ActionPropagation::RemoveActionListeners(nb_action_ptr action)
{
	auto& list = m_actionsListeners[action];
	list.clear();
}

void ActionPropagation::RemoveListener(IPluginContext* ctx)
{
	for (auto& methods : m_actionsListeners)
	{
		for (auto& list : methods.second)
		{
			auto& listener = list.second;
			for (auto it = listener.begin(); it != listener.end();)
			{
				if (it->fn->GetParentRuntime()->GetDefaultContext() == ctx)
				{
					it = listener.erase(it);
				}
				else
				{
					it++;
				}
			}
		}
	}
}