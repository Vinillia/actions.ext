#include "actions/manager/manager.h"
#include "actions/propagation/propagation.h"
#include "actions/processor/vtableswap.h"
#include "actions/processor/processor.h"

#include <amtl/am-hashmap.h>
#include <amtl/am-hashtable.h>
#include <CDetour/detours.h>

#include <algorithm>

CDetour* g_pDestructorLock = nullptr;
extern ActionProcessorShared* g_pActionProcessor;

std::unique_ptr<ProcessorFunctions> gProcessorFunctions;

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

void SetupDestructor()
{
	auto get_vtable = [](void* object) -> void**
	{
		return *reinterpret_cast<void***>(object);
	};

	static bool first = true;
	if (first)
	{
		first = false;

#ifdef _WIN32
		g_pDestructorLock = CDetourManager::CreateDetour(GET_MEMBER_CALLBACK(ActionProcessor__Destructor), GET_MEMBER_TRAMPOLINE(ActionProcessor__Destructor), (void*)(get_vtable(g_pActionProcessor)[0]));
#else		
		g_pDestructorLock = CDetourManager::CreateDetour(GET_MEMBER_CALLBACK(ActionProcessor__Destructor), GET_MEMBER_TRAMPOLINE(ActionProcessor__Destructor), (void*)(get_vtable(g_pActionProcessor)[1]));
#endif
		if (!g_pDestructorLock)
		{
			WarningSM("Failed to create ActionProcessor__Destructor detour");
		}

		g_pDestructorLock->EnableDetour();
	}
}

bool BeginActionProcessing(nb_action_ptr action)
{
	SetupDestructor();
	g_swap_manager.BeginActionProcessing(action);
	return true;
}

bool StopActionProcessing(nb_action_ptr action)
{
	g_swap_manager.StopActionProcessing(action);
	return true;
}

void StopActionProcessing()
{
	g_swap_manager.StopActionProcessing();

	if (g_pDestructorLock)
	{
		g_pDestructorLock->Destroy();
		g_pDestructorLock = nullptr;
	}
}

Autoswap::Autoswap(const void* action, const ProcessorFunction* hf)
{
	m_action = reinterpret_cast<nb_action_ptr>(const_cast<void*>(action));
	m_hashFunction = hf;
	g_swap_manager.SwapAction(m_action, hf);
}

Autoswap::~Autoswap()
{
	g_swap_manager.UnSwapAction(m_action, m_hashFunction);
}

ProcessorFunction::ProcessorFunction(const char* functionNmae, bool contextual) : is_contextual(contextual)
{
	std::string name(action_processor::prefix);

	name.append(functionNmae);

	IGameConfig* config = g_sdkActions.GetGameConfig();
	
	if (!config)
		throw std::runtime_error("Failed to get game config");

	if (config->GetOffset(name.c_str(), &offset))
	{
		hash = compile::hash(name.c_str());
	}
}