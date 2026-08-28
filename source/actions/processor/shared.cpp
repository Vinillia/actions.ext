#include "actions/processor/shared.h"

void __action_swap_vtable(void* action);
void __action_unswap_vtable(void* action);

ActionProcessorShared* g_pActionProcessor = nullptr;

ActionProcessorShared::ActionProcessorShared()
{
	AssertFatalMsg(g_pActionProcessor == nullptr, "Only one ActionProcessor instance allowed!");
	g_pActionProcessor = this;
}

//-----------------------------------------------------------------------------
// Shared Handlers
//-----------------------------------------------------------------------------

INextBotEventResponder* ActionProcessorShared::FirstContainedResponder(void) const
{
	Autoswap guard(this, &gProcessorFunctions->FirstContainedResponder);
	return this->FirstContainedResponder();
}

INextBotEventResponder* ActionProcessorShared::NextContainedResponder(INextBotEventResponder* current) const
{
	Autoswap guard(this, &gProcessorFunctions->NextContainedResponder);
	return this->NextContainedResponder(current);
}

const char* ActionProcessorShared::GetName(void) const
{
	Autoswap guard(this, &gProcessorFunctions->GetName);
	return this->GetName();
}

bool ActionProcessorShared::IsNamed(const char* name) const
{
	Autoswap guard(this, &gProcessorFunctions->IsNamed);
	return this->IsNamed(name);
}

const char* ActionProcessorShared::GetFullName(void) const
{
	Autoswap guard(this, &gProcessorFunctions->GetFullName);
	return this->GetFullName();
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnStart(CBaseEntity* me, Action< CBaseEntity >* priorAction)
{
	return ProcessHandler(gProcessorFunctions->OnStart, this, &ActionProcessorShared::OnStart, me, priorAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::Update(CBaseEntity* me, float interval)
{
	return ProcessHandler(gProcessorFunctions->Update, this, &ActionProcessorShared::Update, me, interval);
}

void ActionProcessorShared::OnEnd(CBaseEntity* me, Action< CBaseEntity >* nextAction)
{
	g_actionsManager.SetActionActor(this, nullptr);
	g_actionsManager.SetActionActor(nextAction, me);
	
	ProcessHandlerEx(gProcessorFunctions->OnEnd, this, &ActionProcessorShared::OnEnd, me, nextAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnSuspend(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	return ProcessHandler(gProcessorFunctions->OnSuspend, this, &ActionProcessorShared::OnSuspend, me, interruptingAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnResume(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	return ProcessHandler(gProcessorFunctions->OnResume, this, &ActionProcessorShared::OnResume, me, interruptingAction);
}

Action< CBaseEntity >* ActionProcessorShared::InitialContainedAction(CBaseEntity* me)
{
	Execution<Action<CBaseEntity>*> execution = ProcessHandlerEx(gProcessorFunctions->InitialContainedAction, this, &ActionProcessorShared::InitialContainedAction, me);
	g_actionsManager.ProcessInitialContainedAction(execution.handler.result_type, this, execution.handler.result, execution.result);
	return execution.result;
}