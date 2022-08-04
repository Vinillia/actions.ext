#include <utility>
#include <map>

#include "extension.h"
#include "actions_propagate.h"
#include "offset_manager.h"

extern OffsetManager* GetOffsetsManager();

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

bool ActionsPropagate::AddListener(int32_t vtableidx, Action* action, IPluginFunction* listener)
{
	auto i = m_handlers.findForAdd(action);

	if (!i.found())
	{
		m_handlers.add(i, action, HandlersArray(SIZE));
	}
	else if (FindListener(vtableidx, action, listener))
	{
		return false;
	}

	i->value[vtableidx].push_back(listener);
	return true;
}

bool ActionsPropagate::RemoveListener(int32_t vtableidx, Action* action, IPluginFunction* listener)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return false;

	PluginCallbacks::iterator iterator;
	if (FindListener(vtableidx, action, listener, &iterator))
	{
		auto& listeners = r->value[vtableidx];
		listeners.erase(iterator);

		if (listeners.size() == 0)
			m_handlers.remove(r);

		return true;
	}

	return false;
}

bool ActionsPropagate::RemoveListener(int32_t vtableidx, Action* action, IPluginContext* context)
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
		
		if (listeners.size() == 0)
			m_handlers.remove(r);
		
		return true;
	}

	return false;
}

void ActionsPropagate::RemoveListeners(Action* action)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return;
	
	m_handlers.remove(r);
	/*
	const int32_t size = r->value.size();
	for (int32_t i = 0; i < size; i++)
	{
		r->value[i].clear();
	}
	*/
}

void ActionsPropagate::RemoveListeners(Action* action, IPluginContext* context)
{
	auto r = m_handlers.find(action);

	if (!r.found())
		return;
	
	m_handlers.remove(r);

	/*
	const int32_t size = r->value.size();
	for (int32_t i = 0; i < size; i++)
	{
		RemoveListener(i, action, context);
	}
	*/
}

void ActionsPropagate::RemoveListeners(IPluginContext* context)
{
	auto iter = m_handlers.iter();

	while (!iter.empty())
	{
		RemoveListeners(iter->key, context);
		iter.next();
	}
}

bool ActionsPropagate::FindListener(int32_t vtableidx, Action* action, IPluginFunction* listener, PluginCallbacks::iterator* iterator)
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

bool ActionsPropagate::FindListener(int32_t vtableidx, IPluginFunction* listener, PluginCallbacks::iterator* iterator)
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

void ActionsPropagate::Dump()
{
	auto iter = m_handlers.iter();

	while (!iter.empty())
	{
        LOG("/----------------------------------/");
		LOG("Dump of %s(%X) action listeners:", iter->key->GetName(), iter->key);

		auto& action_handlers = iter->value;

		const int32_t size = action_handlers.size();
		for (int32_t i = 0; i < size; i++)
		{
			if (!action_handlers[i].size())
				continue;

			int32_t count = 0;
			auto& listeners = action_handlers[i];
			for(auto plistener = listeners.cbegin(); plistener != listeners.cend(); plistener++)
			{
				auto listener = *plistener;
				LOG("%i. Function: %s, Handler: %s, Vtableindex: %i", ++count, listener->DebugName(), GetOffsetsManager()->OffsetToName(i), i);
			}
		}

		iter.next();
	}
}