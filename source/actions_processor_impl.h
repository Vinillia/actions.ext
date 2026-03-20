#pragma once

#ifndef _INCLUDE_ACTIONS_PROCESSOR_IMPL
#define _INCLUDE_ACTIONS_PROCESSOR_IMPL

#include "actions_propagation.h"
#include "actions_pubvars.h"
#include "actionsdefs.h"

class ActionProcessorShared;

struct HashFunction
{
	HashFunction() = default;
	HashFunction(const char* name, bool contextual = false);

	HashValue hash = 0;
	int32_t offset = 0;

	bool is_contextual = false;
};

struct ProcessorFunctions
{
	HashFunction FirstContainedResponder = HashFunction("&ActionProcessor::FirstContainedResponder");
	HashFunction NextContainedResponder = HashFunction("&ActionProcessor::NextContainedResponder");
	HashFunction GetName = HashFunction("&ActionProcessor::GetName");
	HashFunction IsNamed = HashFunction("&ActionProcessor::IsNamed");
	HashFunction GetFullName = HashFunction("&ActionProcessor::GetFullName");

	HashFunction OnStart = HashFunction("&ActionProcessor::OnStart");
	HashFunction Update = HashFunction("&ActionProcessor::Update");
	HashFunction OnEnd = HashFunction("&ActionProcessor::OnEnd");
	HashFunction OnSuspend = HashFunction("&ActionProcessor::OnSuspend");
	HashFunction OnResume = HashFunction("&ActionProcessor::OnResume");
	HashFunction InitialContainedAction = HashFunction("&ActionProcessor::InitialContainedAction");

	HashFunction OnLeaveGround = HashFunction("&ActionProcessor::OnLeaveGround");
	HashFunction OnLandOnGround = HashFunction("&ActionProcessor::OnLandOnGround");
	HashFunction OnContact = HashFunction("&ActionProcessor::OnContact");
	HashFunction OnMoveToSuccess = HashFunction("&ActionProcessor::OnMoveToSuccess");
	HashFunction OnMoveToFailure = HashFunction("&ActionProcessor::OnMoveToFailure");
	HashFunction OnStuck = HashFunction("&ActionProcessor::OnStuck");
	HashFunction OnUnStuck = HashFunction("&ActionProcessor::OnUnStuck");
	HashFunction OnPostureChanged = HashFunction("&ActionProcessor::OnPostureChanged");
	HashFunction OnAnimationActivityComplete = HashFunction("&ActionProcessor::OnAnimationActivityComplete");
	HashFunction OnAnimationActivityInterrupted = HashFunction("&ActionProcessor::OnAnimationActivityInterrupted");
	HashFunction OnAnimationEvent = HashFunction("&ActionProcessor::OnAnimationEvent");
	HashFunction OnIgnite = HashFunction("&ActionProcessor::OnIgnite");
	HashFunction OnInjured = HashFunction("&ActionProcessor::OnInjured");
	HashFunction OnKilled = HashFunction("&ActionProcessor::OnKilled");
	HashFunction OnOtherKilled = HashFunction("&ActionProcessor::OnOtherKilled");
	HashFunction OnSight = HashFunction("&ActionProcessor::OnSight");
	HashFunction OnLostSight = HashFunction("&ActionProcessor::OnLostSight");
	HashFunction OnThreatChanged = HashFunction("&ActionProcessor::OnThreatChanged");
	HashFunction OnSound = HashFunction("&ActionProcessor::OnSound");
	HashFunction OnSpokeConcept = HashFunction("&ActionProcessor::OnSpokeConcept");
	HashFunction OnNavAreaChanged = HashFunction("&ActionProcessor::OnNavAreaChanged");
	HashFunction OnModelChanged = HashFunction("&ActionProcessor::OnModelChanged");
	HashFunction OnPickUp = HashFunction("&ActionProcessor::OnPickUp");
	HashFunction OnDrop = HashFunction("&ActionProcessor::OnDrop");
	HashFunction OnShoved = HashFunction("&ActionProcessor::OnShoved");
	HashFunction OnBlinded = HashFunction("&ActionProcessor::OnBlinded");
	HashFunction OnEnteredSpit = HashFunction("&ActionProcessor::OnEnteredSpit");
	HashFunction OnHitByVomitJar = HashFunction("&ActionProcessor::OnHitByVomitJar");
	HashFunction OnCommandAttack = HashFunction("&ActionProcessor::OnCommandAttack");
	HashFunction OnCommandAssault = HashFunction("&ActionProcessor::OnCommandAssault");
	HashFunction OnCommandApproachByVector = HashFunction("&ActionProcessor::OnCommandApproachByVector");
	HashFunction OnCommandApproachByEntity = HashFunction("&ActionProcessor::OnCommandApproachByEntity");
	HashFunction OnCommandRetreat = HashFunction("&ActionProcessor::OnCommandRetreat");
	HashFunction OnCommandPause = HashFunction("&ActionProcessor::OnCommandPause");
	HashFunction OnCommandResume = HashFunction("&ActionProcessor::OnCommandResume");
	HashFunction OnCommandString = HashFunction("&ActionProcessor::OnCommandString");

	HashFunction IsAbleToBlockMovementOf = HashFunction("&ActionProcessor::IsAbleToBlockMovementOf", true);
	HashFunction ShouldPickUp = HashFunction("&ActionProcessor::ShouldPickUp", true);
	HashFunction ShouldHurry = HashFunction("&ActionProcessor::ShouldHurry", true);
	HashFunction IsHindrance = HashFunction("&ActionProcessor::IsHindrance", true);
	HashFunction SelectTargetPoint = HashFunction("&ActionProcessor::SelectTargetPoint", true);
	HashFunction IsPositionAllowed = HashFunction("&ActionProcessor::IsPositionAllowed", true);
	HashFunction QueryCurrentPath = HashFunction("&ActionProcessor::QueryCurrentPath", true);
	HashFunction SelectMoreDangerousThreat = HashFunction("&ActionProcessor::SelectMoreDangerousThreat", true);
};

extern std::unique_ptr<ProcessorFunctions> gProcessorFunctions;

class Autoswap
{
public:
	Autoswap() = delete;
	Autoswap(const void* action, const HashFunction* hf = nullptr);
	~Autoswap();

private:
	Autoswap(Autoswap&&) = delete;
	Autoswap(const Autoswap&) = delete;
	Autoswap& operator=(const Autoswap&) = delete;
	Autoswap& operator=(Autoswap&&) = delete;

private:
	nb_action_ptr m_action;
	const HashFunction* m_hashFunction;
};

bool BeginActionProcessing(nb_action_ptr action);
bool StopActionProcessing(nb_action_ptr action);
void StopActionProcessing();

#endif // !_INCLUDE_ACTIONS_PROCESSOR_IMPL