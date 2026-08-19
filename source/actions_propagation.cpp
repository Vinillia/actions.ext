#include "actions_propagation.h"
#include "extension.h"

ActionPropagation g_actionsPropagationPre;
ActionPropagation g_actionsPropagationPost;

ListenerVector* ActionPropagation::FindListeners(nb_action_ptr action, HashValue hash) noexcept
{
	auto actionIt = m_actionsListeners.find(action);
	if (actionIt == m_actionsListeners.end())
		return nullptr;

	auto methodIt = actionIt->second.find(hash);
	if (methodIt == actionIt->second.end())
		return nullptr;

	return &methodIt->second;
}

bool ActionPropagation::MarkRemoved(ActionListener& listener) noexcept
{
	if (listener.removed)
		return false;

	listener.removed = true;
	m_cleanupPending = true;
	return true;
}

void ActionPropagation::CleanupRemovedListeners() noexcept
{
	if (m_executionDepth != 0 || !m_cleanupPending)
		return;

	for (auto actionIt = m_actionsListeners.begin(); actionIt != m_actionsListeners.end();)
	{
		auto& methods = actionIt->second;

		for (auto methodIt = methods.begin(); methodIt != methods.end();)
		{
			auto& listeners = methodIt->second;

			// std::erase_if 

			listeners.erase(
				std::remove_if(listeners.begin(), listeners.end(), [](const ActionListener& listener)
				{
					return listener.removed;
				}),
				listeners.end());

			if (listeners.empty())
				methodIt = methods.erase(methodIt);
			else
				++methodIt;
		}

		if (methods.empty())
			actionIt = m_actionsListeners.erase(actionIt);
		else
			++actionIt;
	}

	m_cleanupPending = false;
}

bool ActionPropagation::AddListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	IPluginContext* context = fn->GetParentRuntime()->GetDefaultContext();

	if (!g_actionsManager.IsValidAction(action))
	{
		context->ReportError("Attempt to hook invalid action");
		return false;
	}

	if (!hash)
	{
		context->ReportError("Invalid hash value");
		return false;
	}

	CleanupRemovedListeners();

	auto& listeners = m_actionsListeners[action][hash];
	const auto listener = std::find_if(listeners.cbegin(), listeners.cend(), [fn](const ActionListener& listener)
	{
		return !listener.removed && listener.fn == fn;
	});

	if (listener != listeners.cend())
		return false;

	listeners.emplace_back(hash, fn, context);
	return true;
}

bool ActionPropagation::RemoveListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	auto* listeners = FindListeners(action, hash);
	if (!listeners)
		return false;

	const auto listener = std::find_if(listeners->begin(), listeners->end(), [fn](const ActionListener& listener)
	{
		return !listener.removed && listener.fn == fn;
	});

	if (listener == listeners->end())
		return false;

	MarkRemoved(*listener);
	CleanupRemovedListeners();
	return true;
}

bool ActionPropagation::RemoveListener(nb_action_ptr action, HashValue hash, IPluginContext* ctx)
{
	auto* listeners = FindListeners(action, hash);
	if (!listeners)
		return false;

	const auto listener = std::find_if(listeners->begin(), listeners->end(), [ctx](const ActionListener& listener)
	{
		return !listener.removed && listener.context == ctx;
	});

	if (listener == listeners->end())
		return false;

	MarkRemoved(*listener);
	CleanupRemovedListeners();
	return true;
}

ActionListener* ActionPropagation::HasListener(nb_action_ptr action, HashValue hash, IPluginFunction* fn)
{
	auto* listeners = FindListeners(action, hash);
	if (!listeners)
		return nullptr;

	const auto listener = std::find_if(listeners->begin(), listeners->end(), [fn](const ActionListener& listener)
	{
		return !listener.removed && listener.fn == fn;
	});

	return listener != listeners->end() ? &*listener : nullptr;
}

void ActionPropagation::RemoveActionListeners(nb_action_ptr action)
{
	auto actionIt = m_actionsListeners.find(action);
	if (actionIt == m_actionsListeners.end())
		return;

	if (m_executionDepth == 0)
	{
		m_actionsListeners.erase(actionIt);
		return;
	}

	for (auto& method : actionIt->second)
	{
		for (auto& listener : method.second)
			MarkRemoved(listener);
	}
}

void ActionPropagation::RemoveListener(IPluginContext* ctx)
{
	if (!ctx)
		return;

	for (auto& action : m_actionsListeners)
	{
		for (auto& method : action.second)
		{
			for (auto& listener : method.second)
			{
				if (!listener.removed && listener.context == ctx)
					MarkRemoved(listener);
			}
		}
	}

	CleanupRemovedListeners();
}
