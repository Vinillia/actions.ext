#include "actions_processor_shared.h"

void __action_swap_vtable(ActionProcessorShared* action);
void __action_unswap_vtable(ActionProcessorShared* action);

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
	Autoswap guard(this);
	return this->FirstContainedResponder();
}

INextBotEventResponder* ActionProcessorShared::NextContainedResponder(INextBotEventResponder* current) const
{
	Autoswap guard(this);
	return this->NextContainedResponder(current);
}

const char* ActionProcessorShared::GetName(void) const
{
	Autoswap guard(this);
	return this->GetName();
}

bool ActionProcessorShared::IsNamed(const char* name) const
{
	Autoswap guard(this);
	return this->IsNamed(name);
}

const char* ActionProcessorShared::GetFullName(void) const
{
	Autoswap guard(this);
	return this->GetFullName();
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnStart(CBaseEntity* me, Action< CBaseEntity >* priorAction)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnStart");
	return ProcessHandler(hash, this, &ActionProcessorShared::OnStart, me, priorAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::Update(CBaseEntity* me, float interval)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::Update");
	return ProcessHandler(hash, this, &ActionProcessorShared::Update, me, interval);
}

void ActionProcessorShared::OnEnd(CBaseEntity* me, Action< CBaseEntity >* nextAction)
{
	g_actionsManager.SetActionActor(this, nullptr);
	g_actionsManager.SetActionActor(nextAction, me);
	 
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnEnd");
	ProcessHandlerEx(hash, this, &ActionProcessorShared::OnEnd, me, nextAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnSuspend(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSuspend");
	return ProcessHandler(hash, this, &ActionProcessorShared::OnSuspend, me, interruptingAction);
}

ActionResult< CBaseEntity >	ActionProcessorShared::OnResume(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::OnResume");
	return ProcessHandler(hash, this, &ActionProcessorShared::OnResume, me, interruptingAction);
}

Action< CBaseEntity >* ActionProcessorShared::InitialContainedAction(CBaseEntity* me)
{
	constexpr HashValue hash = compile::hash("&ActionProcessor::InitialContainedAction");
	execution_result<Action<CBaseEntity>*> er = ProcessHandlerEx(hash, this, &ActionProcessorShared::InitialContainedAction, me);
	g_actionsManager.ProcessInitialContainedAction(er.rt(), this, er.rv(), er.effectiveResult);
	return er.effectiveResult;
}