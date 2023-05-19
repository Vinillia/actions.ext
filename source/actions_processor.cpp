#include "actions_manager.h"
#include "actions_propagation.h"
#include "actions_vtableswap.h"
#include "actions_processor.h"

#include <amtl/am-hashmap.h>
#include <amtl/am-hashtable.h>
#include <CDetour/detours.h>

#include <algorithm>

#define AUTO_SWAP_GUARD() AutoswapGuard guard(this)

ActionProcessor g_ActionProcessor;
ActionProcessor* g_pActionProcessor = &g_ActionProcessor;
CDetour* g_pDestructorLock = nullptr;

class AutoswapGuard
{
public:
	AutoswapGuard() = delete;
	AutoswapGuard(const ActionProcessor* action)
	{
		m_action = const_cast<ActionProcessor*>(action);
		
		auto r = g_virtualMap.find((nb_action_ptr)action);

		if (!r.found())
		{
			Warning("AutoswapGuard couldn't find vtable");
			return;
		}

		__internal_data data = r->value;
		vtable_swap(m_action, &data);
	}

	~AutoswapGuard()
	{
		vtable_swap(m_action, g_pActionProcessor);
	}

private:
	AutoswapGuard(AutoswapGuard&&) = delete;
	AutoswapGuard(const AutoswapGuard&) = delete;
	AutoswapGuard& operator=(const AutoswapGuard&) = delete;
	AutoswapGuard& operator=(AutoswapGuard&&) = delete;
private:
	ActionProcessor* m_action;
};


template<typename RETURN, typename METHOD, typename ...Args>
auto Process(METHOD&& method, nb_action_ptr action, HashValue hash, RETURN& result, Args... args)
{
	ResultType plpre = Pl_Continue, plpost = Pl_Continue;
	action_process_t<RETURN> process = {};

	g_actionsManager.PushRuntimeResult(&result);
	plpre = g_actionsPropagationPre.ProcessMethod<RETURN, Args...>(action, hash, &result, args...);

	if (plpre != Pl_Handled)
	{
		process.returnValue = method();

		if constexpr (is_action_result_v<RETURN>)
		{
			if (process.returnValue.m_action)
				g_actionsManager.AddPending(process.returnValue.m_action);
		}

		if (plpre < Pl_Changed)
			result = process.returnValue;
	}

	plpost = g_actionsPropagationPost.ProcessMethod<RETURN, Args...>(action, hash, &result, args...);
	g_actionsManager.PopRuntimeResult();

	if (plpre < Pl_Changed && plpost < Pl_Changed)
		result = process.returnValue;

	if constexpr (is_action_result_v<RETURN>)
	{
		if (result.m_action == nullptr && (result.m_type == CHANGE_TO || result.m_type == SUSPEND_FOR))
			result = process.returnValue;

		g_actionsManager.ProcessResult(action, reinterpret_cast<const ActionResult<CBaseEntity>&>(result));
	}

	process.resultType = std::max<decltype(process.resultType)>(plpre, plpost);
	return process;
}


//-----------------------------------------------------------------------------
// Processing
//-----------------------------------------------------------------------------

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
			Warning("Failed to create ActionProcessor__Destructor detour");
			return false;
		}

		g_pDestructorLock->EnableDetour();
	}

	auto r = g_virtualMap.findForAdd(action);
	if (r.found())
	{
		// Warning("BeginActionProcessing called two times!");
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
		// Warning("StopActionProcessing failed to find action!");
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

//-----------------------------------------------------------------------------
// Action Handlers
//-----------------------------------------------------------------------------

ActionProcessor::ActionProcessor()
{
}

ActionProcessor::~ActionProcessor()
{
}

INextBotEventResponder* ActionProcessor::FirstContainedResponder(void) const
{
	AUTO_SWAP_GUARD();
	return this->FirstContainedResponder();
}

INextBotEventResponder* ActionProcessor::NextContainedResponder(INextBotEventResponder* current) const
{
	AUTO_SWAP_GUARD();
	return this->NextContainedResponder(current);
}

const char* ActionProcessor::GetName(void) const
{
	AUTO_SWAP_GUARD();
	return this->GetName();
}

bool ActionProcessor::IsNamed(const char* name) const
{
	AUTO_SWAP_GUARD();
	return this->IsNamed(name);
}

const char* ActionProcessor::GetFullName(void) const
{
	AUTO_SWAP_GUARD();
	return this->GetFullName();
}

ActionResult< CBaseEntity >	ActionProcessor::OnStart(CBaseEntity* me, Action< CBaseEntity >* priorAction)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnStart");
	ActionResult< CBaseEntity > result;
	Process([&, this]() { return this->OnStart(me, priorAction); }, this, hash, result, me, priorAction);
	return result;
}

ActionResult< CBaseEntity >	ActionProcessor::Update(CBaseEntity* me, float interval)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::Update");
	ActionResult< CBaseEntity > result;
	Process([&, this]() { return this->Update(me, interval); }, this, hash, result, me, interval);
	return result;
}

void ActionProcessor::OnEnd(CBaseEntity* me, Action< CBaseEntity >* nextAction)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnEnd");
	int result;
	Process([&, this]() { this->OnEnd(me, nextAction); return 0; }, this, hash, result, me, nextAction);
}

ActionResult< CBaseEntity >	ActionProcessor::OnSuspend(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSuspend");
	ActionResult< CBaseEntity > result;
	Process([&, this]() { return this->OnSuspend(me, interruptingAction); }, this, hash, result, me, interruptingAction);
	return result;
}

ActionResult< CBaseEntity >	ActionProcessor::OnResume(CBaseEntity* me, Action< CBaseEntity >* interruptingAction)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnResume");
	ActionResult< CBaseEntity > result;
	Process([&, this]() { return this->OnResume(me, interruptingAction); }, this, hash, result, me, interruptingAction);
	return result;
}

Action< CBaseEntity >* ActionProcessor::InitialContainedAction(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::InitialContainedAction");
	Action< CBaseEntity >* result = nullptr;
	auto proces_result = Process([&, this]() { return this->InitialContainedAction(me); }, this, hash, result, me);
	g_actionsManager.ProcessInitialContainedAction(proces_result.resultType, this, proces_result.returnValue, result);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLeaveGround(CBaseEntity* me, CBaseEntity* ground)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLeaveGround");
	EventDesiredResult< CBaseEntity > result;
	Process([&, this]() { return this->OnLeaveGround(me, ground); }, this, hash, result, me, ground);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLandOnGround(CBaseEntity* me, CBaseEntity* ground)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLandOnGround");
	EventDesiredResult< CBaseEntity > result;
	Process([&, this]() { return this->OnLandOnGround(me, ground); }, this, hash, result, me, ground);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnContact(CBaseEntity* me, CBaseEntity* other, CGameTrace* trace)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnContact");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnContact(me, other, trace); }, this, hash, result, me, other, trace);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnMoveToSuccess(CBaseEntity* me, const Path* path)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnMoveToSuccess");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnMoveToSuccess(me, path); }, this, hash, result, me, path);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnMoveToFailure(CBaseEntity* me, const Path* path, MoveToFailureType reason)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnMoveToFailure");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnMoveToFailure(me, path, reason); }, this, hash, result, me, reason);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnStuck(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnStuck");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnStuck(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnUnStuck(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnUnStuck");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnUnStuck(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnPostureChanged(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnPostureChanged");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnPostureChanged(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationActivityComplete(CBaseEntity* me, int activity)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationActivityComplete");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnAnimationActivityComplete(me, activity); }, this, hash, result, me, activity);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationActivityInterrupted(CBaseEntity* me, int activity)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationActivityInterrupted");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnAnimationActivityInterrupted(me, activity); }, this, hash, result, me, activity);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnAnimationEvent(CBaseEntity* me, animevent_t* event)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnAnimationEvent");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnAnimationEvent(me, event); }, this, hash, result, me, event);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnIgnite(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnIgnite");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnIgnite(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnInjured(CBaseEntity* me, const CTakeDamageInfo& info)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnInjured");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnInjured(me, info); }, this, hash, result, me, &info);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnKilled(CBaseEntity* me, const CTakeDamageInfo& info)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnKilled");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnKilled(me, info); }, this, hash, result, me, &info);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnOtherKilled(CBaseEntity* me, CBaseCombatCharacter* victim, const CTakeDamageInfo& info)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnOtherKilled");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnOtherKilled(me, victim, info); }, this, hash, result, me, victim, &info);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSight(CBaseEntity* me, CBaseEntity* subject)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSight");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnSight(me, subject); }, this, hash, result, me, subject);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnLostSight(CBaseEntity* me, CBaseEntity* subject)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnLostSight");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnLostSight(me, subject); }, this, hash, result, me, subject);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnThreatChanged(CBaseEntity* me, CBaseEntity* subject)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnThreatChanged");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnThreatChanged(me, subject); }, this, hash, result, me, subject);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSound(CBaseEntity* me, CBaseEntity* source, const Vector& pos, KeyValues* keys)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSound");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnSound(me, source, pos, keys); }, this, hash, result, me, source, pos, keys);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnSpokeConcept(CBaseEntity* me, CBaseCombatCharacter* who, AIConcept_t aiconcept, AI_Response* response, void* unknown)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnSpokeConcept");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnSpokeConcept(me, who, aiconcept, response, unknown); }, this, hash, result, me, who, aiconcept, unknown);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnNavAreaChanged(CBaseEntity* me, CNavArea* newArea, CNavArea* oldArea)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnNavAreaChanged");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnNavAreaChanged(me, newArea, oldArea); }, this, hash, result, me, newArea, oldArea);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnModelChanged(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnModelChanged");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnModelChanged(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnPickUp(CBaseEntity* me, CBaseEntity* item, CBaseCombatCharacter* giver)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnPickUp");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnPickUp(me, item, giver); }, this, hash, result, me, item, giver);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnDrop(CBaseEntity* me, CBaseEntity* item)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnDrop");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnDrop(me, item); }, this, hash, result, me, item);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnShoved(CBaseEntity* me, CBaseEntity* pusher)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnShoved");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnShoved(me, pusher); }, this, hash, result, me, pusher);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnBlinded(CBaseEntity* me, CBaseEntity* blinder)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnBlinded");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnShoved(me, blinder); }, this, hash, result, me, blinder);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnEnteredSpit(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnEnteredSpit");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnEnteredSpit(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnHitByVomitJar(CBaseEntity* me, CBaseEntity* owner)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnHitByVomitJar");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnHitByVomitJar(me, owner); }, this, hash, result, me, owner);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandAttack(CBaseEntity* me, CBaseEntity* victim)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandAttack");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandAttack(me, victim); }, this, hash, result, me, victim);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandAssault(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandAssault");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandAssault(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandApproach(CBaseEntity* me, const Vector& pos, float range)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandApproachByVector");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandApproach(me, pos, range); }, this, hash, result, me, pos, range);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandApproach(CBaseEntity* me, CBaseEntity* goal)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandApproachByEntity");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandApproach(me, goal); }, this, hash, result, me, goal);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandRetreat(CBaseEntity* me, CBaseEntity* threat, float range)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandRetreat");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandRetreat(me, threat, range); }, this, hash, result, me, threat, range);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandPause(CBaseEntity* me, float duration)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandPause");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandPause(me, duration); }, this, hash, result, me, duration);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandResume(CBaseEntity* me)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandResume");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandResume(me); }, this, hash, result, me);
	return result;
}

EventDesiredResult< CBaseEntity > ActionProcessor::OnCommandString(CBaseEntity* me, const char* command)
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::OnCommandString");
	EventDesiredResult<CBaseEntity> result;
	Process([&, this]() { return this->OnCommandString(me, command); }, this, hash, result, me, command);
	return result;
}

bool ActionProcessor::IsAbleToBlockMovementOf(const INextBot* botInMotion) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::IsAbleToBlockMovementOf");
	bool result;
	Process([&, this]() { return this->IsAbleToBlockMovementOf(botInMotion); }, const_cast<ActionProcessor*>(this), hash, result, botInMotion);
	return result;
}

QueryResultType ActionProcessor::ShouldPickUp(const INextBot* me, CBaseEntity* item) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldPickUp");
	QueryResultType result;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->ShouldPickUp(me, item); }, const_cast<ActionProcessor*>(this), hash, result, me, item);
	return result;
}

QueryResultType ActionProcessor::ShouldHurry(const INextBot* me) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::ShouldHurry");
	QueryResultType result;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->ShouldHurry(me); }, const_cast<ActionProcessor*>(this), hash, result, me);
	return result;
}

QueryResultType ActionProcessor::IsHindrance(const INextBot* me, CBaseEntity* blocker) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::IsHindrance");
	QueryResultType result;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->IsHindrance(me, blocker); }, const_cast<ActionProcessor*>(this), hash, result, me, blocker);
	return result;
}

Vector ActionProcessor::SelectTargetPoint(const INextBot* me, const CBaseCombatCharacter* subject) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::SelectTargetPoint");
	Vector result;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->SelectTargetPoint(me, subject); }, const_cast<ActionProcessor*>(this), hash, result, me, subject);
	return result;
}

QueryResultType ActionProcessor::IsPositionAllowed(const INextBot* me, const Vector& pos) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::IsPositionAllowed");
	QueryResultType result;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->IsPositionAllowed(me, pos); }, const_cast<ActionProcessor*>(this), hash, result, me, pos);
	return result;
}

PathFollower* ActionProcessor::QueryCurrentPath(const INextBot* me) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::QueryCurrentPath");
	PathFollower* result = nullptr;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->QueryCurrentPath(me); }, const_cast<ActionProcessor*>(this), hash, result, me);
	return result;
}

const CKnownEntity* ActionProcessor::SelectMoreDangerousThreat(const INextBot* me, const CBaseCombatCharacter* subject, const CKnownEntity* threat1, const CKnownEntity* threat2) const
{
	AUTO_SWAP_GUARD();

	constexpr HashValue hash = compile::hash("&ActionProcessor::SelectMoreDangerousThreat");
	const CKnownEntity* result = nullptr;
	Process([&, this]() { return static_cast<const IContextualQuery*>(this)->SelectMoreDangerousThreat(me, subject, threat1, threat2); }, const_cast<ActionProcessor*>(this), hash, result, me, subject, threat1, threat2);
	return result;
}