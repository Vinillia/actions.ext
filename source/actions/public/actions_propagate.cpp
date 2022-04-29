#include <utility>

#include "extension.h"
#include "actions_propagate.h"

ActionsPropagate* g_pActionsPropagatePre = new ActionsPropagate();
ActionsPropagate* g_pActionsPropagatePost = new ActionsPropagate();

ActionsPropagate::ActionsPropagate()
{
	m_init = m_handlers.init();

	if (!m_init)
	{
		LOGERROR("Failed to init action handlers");
	}
}

bool ActionsPropagate::AddListener(size_t vtableidx, Action* action, IPluginFunction* listener)
{
	auto i = m_handlers.findForAdd(action);

	if (!i.found())
	{
		m_handlers.add(i, action, std::move(HandlersArray(SIZE)));
	}
	else if (FindListener(vtableidx, action, listener))
	{
		return false;
	}

	i->value[vtableidx].push_back(listener);
	return true;
}

bool ActionsPropagate::RemoveListener(size_t vtableidx, Action* action, IPluginFunction* listener)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return false;

	PluginCallbacks::iterator iterator;
	if (FindListener(vtableidx, action, listener, &iterator))
	{
		auto& listeners = r->value[vtableidx];
		listeners.erase(iterator);

		return true;
	}

	return false;
}

bool ActionsPropagate::RemoveListener(size_t vtableidx, Action* action, IPluginContext* context)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return false;
	
	auto& listeners = r->value[vtableidx];

	for (auto iter = listeners.begin(); iter != listeners.end(); iter++)
	{
		if ((*iter)->GetParentRuntime()->GetDefaultContext() != context)
			continue;

		listeners.erase(iter);
		return true;
	}

	return false;
}

void ActionsPropagate::RemoveListeners(Action* action)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return;
	
	const size_t size = r->value.size();
	for (size_t i = 0; i < size; i++)
	{
		r->value[i].clear();
	}
}

void ActionsPropagate::RemoveListeners(Action* action, IPluginContext* context)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return;
	
	const size_t size = r->value.size();
	for (size_t i = 0; i < size; i++)
	{
		RemoveListener(i, action, context);
	}
}

void ActionsPropagate::RemoveListeners(IPluginContext* context)
{
	auto iter = m_handlers.iter();

	while (!iter.empty())
	{
		const size_t size = iter->value.size();
		for (size_t i = 0; i < size; i++)
		{
			RemoveListener(i, iter->key, context);
		}

		iter.next();
	}
}

bool ActionsPropagate::FindListener(size_t vtableidx, Action* action, IPluginFunction* listener, PluginCallbacks::iterator* iterator)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return false;

	auto& listeners = r->value[vtableidx];

	for (auto iter = listeners.begin(); iter != listeners.end(); iter++)
	{
		if (*iter != listener)
			continue;
		
		if (iterator)
			*iterator = iter;

		return true;
	}

	return false;
}

bool ActionsPropagate::FindListener(size_t vtableidx, IPluginFunction* listener, PluginCallbacks::iterator* iterator)
{
	auto iter = m_handlers.iter();

	while (!iter.empty())
	{
		if (FindListener(vtableidx, iter->key, listener, iterator))
		{
			return true;
		}

		iter.next();
	}

	return false;
}

void ActionsPropagate::OnActionAdded(Action* action)
{

}

void ActionsPropagate::OnActionDestroyed(Action* action)
{
	g_pActionsPropagatePre->RemoveListeners(action);
	g_pActionsPropagatePost->RemoveListeners(action);
}