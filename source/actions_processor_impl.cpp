#include "actions_manager.h"
#include "actions_propagation.h"
#include "actions_vtableswap.h"
#include "actions_processor_impl.h"

#include <amtl/am-hashmap.h>
#include <amtl/am-hashtable.h>
#include <CDetour/detours.h>

#include <algorithm>

CDetour* g_pDestructorLock = nullptr;
extern ActionProcessorShared* g_pActionProcessor;

void __action_swap_vtable(ActionProcessorShared* action)
{
	auto r = g_virtualMap.find((nb_action_ptr)action);

	if (!r.found())
	{
		WarningSM("Autoswap couldn't find vtable");
		return;
	}

	__internal_data data = r->value;
	vtable_swap(action, &data);
}

void __action_unswap_vtable(ActionProcessorShared* action)
{
	vtable_swap(action, g_pActionProcessor);
}

#ifdef _WIN32
DETOUR_DECL_MEMBER1(ActionProcessor__Destructor, void, bool, flag)
#else
DETOUR_DECL_MEMBER0(ActionProcessor__Destructor, void)
#endif
{
	nb_action_ptr action = (nb_action_ptr)this;
	if (action == (nb_action_ptr)g_pActionProcessor)
	{
#ifdef _WIN32
		DETOUR_MEMBER_CALL(ActionProcessor__Destructor)(flag);
#else
		DETOUR_MEMBER_CALL(ActionProcessor__Destructor)();
#endif
		return;
	}

	g_actionsManager.Remove(action);
	//action->nb_action::~nb_action();
	delete action;
}

bool BeginActionProcessing(nb_action_ptr action)
{
	if (!g_pDestructorLock)
	{
#ifdef _WIN32
		g_pDestructorLock = CDetourManager::CreateDetour(GET_MEMBER_CALLBACK(ActionProcessor__Destructor), GET_MEMBER_TRAMPOLINE(ActionProcessor__Destructor), (void*)(vtable_get00(g_pActionProcessor)[0]));
#else		
		g_pDestructorLock = CDetourManager::CreateDetour(GET_MEMBER_CALLBACK(ActionProcessor__Destructor), GET_MEMBER_TRAMPOLINE(ActionProcessor__Destructor), (void*)(vtable_get00(g_pActionProcessor)[1]));
#endif
		if (!g_pDestructorLock)
		{
			WarningSM("Failed to create ActionProcessor__Destructor detour");
			return false;
		}

		g_pDestructorLock->EnableDetour();
	}

	auto r = g_virtualMap.findForAdd(action);
	if (r.found())
	{
		// WarningSM("BeginActionProcessing called two times!");
		return false;
	}

	__internal_data data = { vtable_get00(action), vtable_get01(action) };
	g_virtualMap.add(r, action, data);
	vtable_swap(action, g_pActionProcessor);
	return true;
}

bool StopActionProcessing(nb_action_ptr action)
{
	auto r = g_virtualMap.find(action);

	if (!r.found())
	{
		// WarningSM("StopActionProcessing failed to find action!");
		return false;
	}

	__internal_data data = r->value;
	vtable_swap(action, &data);
	g_virtualMap.remove(r);
	return true;
}

void StopActionProcessing()
{
	auto iter = g_virtualMap.iter();

	while (!iter.empty())
	{
		StopActionProcessing(iter->key);
		iter.next();
	}

	if (g_pDestructorLock)
	{
		g_pDestructorLock->Destroy();
		g_pDestructorLock = nullptr;
	}
}

Autoswap::Autoswap(const ActionProcessorShared* action)
{
	m_action = const_cast<ActionProcessorShared*>(action);
	__action_swap_vtable(m_action);
}

Autoswap::~Autoswap()
{
	__action_unswap_vtable(m_action);
}

