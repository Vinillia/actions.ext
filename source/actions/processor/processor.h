#ifndef ACTIONS_PROCESSOR_IMPLH
#define ACTIONS_PROCESSOR_IMPLH

#include "actions/defs.h"

#include "actions/propagation/propagation.h"
#include "actions/runtime/pubvars.h"

class ActionProcessorShared;

struct ProcessorFunction
{
public:
    ProcessorFunction() = default;

    static ProcessorFunction Regular(const char* name)
    {
        return ProcessorFunction(name, false);
    }

    static ProcessorFunction Contextual(const char* name)
    {
        return ProcessorFunction(name, true);
    }

    explicit operator bool() const noexcept
    {
        return hash != 0 && offset != 0;
    }

    HashValue hash = 0;
    int32_t offset = 0;
    bool is_contextual = false;

private:
    ProcessorFunction(const char* name, bool contextual);
};

struct ProcessorFunctions
{
    ProcessorFunction FirstContainedResponder = ProcessorFunction::Regular("FirstContainedResponder");
    ProcessorFunction NextContainedResponder = ProcessorFunction::Regular("NextContainedResponder");
    ProcessorFunction GetName = ProcessorFunction::Regular("GetName");
    ProcessorFunction IsNamed = ProcessorFunction::Regular("IsNamed");
    ProcessorFunction GetFullName = ProcessorFunction::Regular("GetFullName");

    ProcessorFunction OnStart = ProcessorFunction::Regular("OnStart");
    ProcessorFunction Update = ProcessorFunction::Regular("Update");
    ProcessorFunction OnEnd = ProcessorFunction::Regular("OnEnd");
    ProcessorFunction OnSuspend = ProcessorFunction::Regular("OnSuspend");
    ProcessorFunction OnResume = ProcessorFunction::Regular("OnResume");
    ProcessorFunction InitialContainedAction = ProcessorFunction::Regular("InitialContainedAction");

    ProcessorFunction OnLeaveGround = ProcessorFunction::Regular("OnLeaveGround");
    ProcessorFunction OnLandOnGround = ProcessorFunction::Regular("OnLandOnGround");
    ProcessorFunction OnContact = ProcessorFunction::Regular("OnContact");
    ProcessorFunction OnMoveToSuccess = ProcessorFunction::Regular("OnMoveToSuccess");
    ProcessorFunction OnMoveToFailure = ProcessorFunction::Regular("OnMoveToFailure");
    ProcessorFunction OnStuck = ProcessorFunction::Regular("OnStuck");
    ProcessorFunction OnUnStuck = ProcessorFunction::Regular("OnUnStuck");
    ProcessorFunction OnPostureChanged = ProcessorFunction::Regular("OnPostureChanged");

    ProcessorFunction OnAnimationActivityComplete = ProcessorFunction::Regular("OnAnimationActivityComplete");
    ProcessorFunction OnAnimationActivityInterrupted = ProcessorFunction::Regular("OnAnimationActivityInterrupted");
    ProcessorFunction OnAnimationEvent = ProcessorFunction::Regular("OnAnimationEvent");

    ProcessorFunction OnIgnite = ProcessorFunction::Regular("OnIgnite");
    ProcessorFunction OnInjured = ProcessorFunction::Regular("OnInjured");
    ProcessorFunction OnKilled = ProcessorFunction::Regular("OnKilled");
    ProcessorFunction OnOtherKilled = ProcessorFunction::Regular("OnOtherKilled");

    ProcessorFunction OnSight = ProcessorFunction::Regular("OnSight");
    ProcessorFunction OnLostSight = ProcessorFunction::Regular("OnLostSight");
    ProcessorFunction OnThreatChanged = ProcessorFunction::Regular("OnThreatChanged");
    ProcessorFunction OnSound = ProcessorFunction::Regular("OnSound");
    ProcessorFunction OnSpokeConcept = ProcessorFunction::Regular("OnSpokeConcept");

    ProcessorFunction OnNavAreaChanged = ProcessorFunction::Regular("OnNavAreaChanged");
    ProcessorFunction OnModelChanged = ProcessorFunction::Regular("OnModelChanged");
    ProcessorFunction OnPickUp = ProcessorFunction::Regular("OnPickUp");
    ProcessorFunction OnDrop = ProcessorFunction::Regular("OnDrop");
    ProcessorFunction OnShoved = ProcessorFunction::Regular("OnShoved");
    ProcessorFunction OnBlinded = ProcessorFunction::Regular("OnBlinded");
    ProcessorFunction OnEnteredSpit = ProcessorFunction::Regular("OnEnteredSpit");
    ProcessorFunction OnHitByVomitJar = ProcessorFunction::Regular("OnHitByVomitJar");

    ProcessorFunction OnCommandAttack = ProcessorFunction::Regular("OnCommandAttack");
    ProcessorFunction OnCommandAssault = ProcessorFunction::Regular("OnCommandAssault");
    ProcessorFunction OnCommandApproachByVector = ProcessorFunction::Regular("OnCommandApproachByVector");
    ProcessorFunction OnCommandApproachByEntity = ProcessorFunction::Regular("OnCommandApproachByEntity");
    ProcessorFunction OnCommandRetreat = ProcessorFunction::Regular("OnCommandRetreat");
    ProcessorFunction OnCommandPause = ProcessorFunction::Regular("OnCommandPause");
    ProcessorFunction OnCommandResume = ProcessorFunction::Regular("OnCommandResume");
    ProcessorFunction OnCommandString = ProcessorFunction::Regular("OnCommandString");

    ProcessorFunction OnWeaponFired = ProcessorFunction::Regular("OnWeaponFired");
    ProcessorFunction OnActorEmoted = ProcessorFunction::Regular("OnActorEmoted");
    ProcessorFunction OnTerritoryContested = ProcessorFunction::Regular("OnTerritoryContested");
    ProcessorFunction OnTerritoryCaptured = ProcessorFunction::Regular("OnTerritoryCaptured");
    ProcessorFunction OnTerritoryLost = ProcessorFunction::Regular("OnTerritoryLost");
    ProcessorFunction OnWin = ProcessorFunction::Regular("OnWin");
    ProcessorFunction OnLose = ProcessorFunction::Regular("OnLose");

    ProcessorFunction IsAbleToBlockMovementOf =
        ProcessorFunction::Contextual("IsAbleToBlockMovementOf");

    ProcessorFunction ShouldPickUp =
        ProcessorFunction::Contextual("ShouldPickUp");

    ProcessorFunction ShouldHurry =
        ProcessorFunction::Contextual("ShouldHurry");

    ProcessorFunction IsHindrance =
        ProcessorFunction::Contextual("IsHindrance");

    ProcessorFunction SelectTargetPoint =
        ProcessorFunction::Contextual("SelectTargetPoint");

    ProcessorFunction IsPositionAllowed =
        ProcessorFunction::Contextual("IsPositionAllowed");

    ProcessorFunction QueryCurrentPath =
        ProcessorFunction::Contextual("QueryCurrentPath");

    ProcessorFunction SelectMoreDangerousThreat =
        ProcessorFunction::Contextual("SelectMoreDangerousThreat");

    ProcessorFunction ShouldAttack =
        ProcessorFunction::Contextual("ShouldAttack");

    ProcessorFunction ShouldRetreat =
        ProcessorFunction::Contextual("ShouldRetreat");
};

extern std::unique_ptr<ProcessorFunctions> gProcessorFunctions;

class Autoswap
{
public:
	Autoswap() = delete;
	Autoswap(const void* action, const ProcessorFunction* hf = nullptr);
	~Autoswap();

private:
	Autoswap(Autoswap&&) = delete;
	Autoswap(const Autoswap&) = delete;
	Autoswap& operator=(const Autoswap&) = delete;
	Autoswap& operator=(Autoswap&&) = delete;

private:
	nb_action_ptr m_action;
	const ProcessorFunction* m_hashFunction;
};

bool BeginActionProcessing(nb_action_ptr action);
bool StopActionProcessing(nb_action_ptr action);
void StopActionProcessing();

#endif // !ACTIONS_PROCESSOR_IMPLH