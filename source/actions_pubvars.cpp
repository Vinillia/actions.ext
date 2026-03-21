#include "actions_pubvars.h"
#include "actions_processor.h"

#include "smsdk_ext.h"
#include "actions_encoders.h"
#include "actionsdefs.h"

ActionPublicsManager g_publicsManager;

ActionPublicsManager::ActionPublicsManager()
{
	m_varMap.init();
}

ActionPublicsManager::~ActionPublicsManager()
{
	m_varMap.clear();
}

HashValue ActionPublicsManager::GetHash(const char* name)
{
	auto r = m_varMap.find(name);

	if (!r.found())
	{
		WarningSM("Failed to find hash (%s)", name);
		return 0;
	}

	return r->value;
}

const char* ActionPublicsManager::GetName(HashValue hash)
{
	auto iter = m_varMap.iter();

	while (!iter.empty())
	{
		if (iter->value == hash)
			return iter->key.data();

		iter.next();
	}

	return "invalid";
}

void ActionPublicsManager::SyncPlugin(IPluginContext* pl)
{
	SyncListeners(pl);
	SyncEncoders(pl);
}

void ActionPublicsManager::SyncListeners(SourcePawn::IPluginContext* pl)
{
	auto iter = m_varMap.iter();

	while (!iter.empty())
	{
		SetPluginPubVar(pl, iter->key.data(), static_cast<cell_t>(iter->value));
		iter.next();
	}
}

void ActionPublicsManager::SyncEncoders(SourcePawn::IPluginContext* pl)
{
#ifdef INCLUDE_ACTIONS_CONSTRUCTOR
	auto encoders = ActionEncoder::GetActionEncoders();

	for (auto it = encoders->cbegin(); it != encoders->cend(); it++)
	{
		const ActionEncoder* encoder = *it;
		SetPluginPubVar(pl, encoder->PublicName(), ToPseudoAddress(encoder));
	}
#endif // INCLUDE_ACTIONS_CONSTRUCTOR
}

bool ActionPublicsManager::SetPluginPubVar(SourcePawn::IPluginContext* pl, const char* name, cell_t value)
{
	uint32_t index;
	int err = pl->GetRuntime()->FindPubvarByName(name, &index);
	if (!err)
	{
		sp_pubvar_t* var;
		pl->GetRuntime()->GetPubvarByIndex(index, &var);
		if (var)
		{
			*var->offs = value;
		}

		return true;
	}

	return false;
}

bool ActionPublicsManager::IsUnique(HashValue value)
{
	auto iter = m_varMap.iter();
	while (!iter.empty())
	{
		if (value == iter->value)
			return false;

		iter.next();
	}

	return true;
}

bool ActionPublicsManager::AddHash(const char* pubvar, HashValue hash, bool force)
{
	auto r = m_varMap.findForAdd(pubvar);

	if (!r.found())
	{
		if (!force && !IsUnique(hash))
		{
			WarningSM("%s isn't unique! (%X)", pubvar, hash);
			return false;
		}

		m_varMap.add(r, pubvar, hash);
	}

	return true;
}

void ActionPublicsManager::InsertMethodHash(
	const char* method,
	const char* value,
	InsertMethodHashMode mode)
{
	std::string pubvar = BuildHashString(pubvar_prefix, method, value, mode, value);
	std::string fn = BuildHashString(method_prefix, method, value, mode, method);

	HashValue hash = compile::hash(fn.c_str());

	g_publicsManager.AddHash(pubvar.c_str(), hash, mode == InsertMethodHashMode::Alias);
}

void ActionPublicsManager::InitializePublicVariables()
{
	InsertMethodHash("OnStart");
	InsertMethodHash("Update");
	InsertMethodHash("OnEnd");
	InsertMethodHash("OnSuspend");
	InsertMethodHash("OnResume");

	InsertMethodHash("InitialContainedAction");

	InsertMethodHash("OnLeaveGround");
	InsertMethodHash("OnLandOnGround");
	InsertMethodHash("OnContact");
	InsertMethodHash("OnMoveToSuccess");
	InsertMethodHash("OnMoveToFailure");
	InsertMethodHash("OnStuck");
	InsertMethodHash("OnUnStuck");
	InsertMethodHash("OnPostureChanged");
	InsertMethodHash("OnAnimationActivityComplete");
	InsertMethodHash("OnAnimationActivityInterrupted");
	InsertMethodHash("OnAnimationEvent");
	InsertMethodHash("OnIgnite");
	InsertMethodHash("OnInjured");
	InsertMethodHash("OnKilled");
	InsertMethodHash("OnOtherKilled");
	InsertMethodHash("OnSight");
	InsertMethodHash("OnLostSight");
	InsertMethodHash("OnThreatChanged");
	InsertMethodHash("OnSound");
	InsertMethodHash("OnSpokeConcept");
	InsertMethodHash("OnNavAreaChanged");
	InsertMethodHash("OnModelChanged");
	InsertMethodHash("OnPickUp");
	InsertMethodHash("OnDrop");
	InsertMethodHash("OnShoved");
	InsertMethodHash("OnBlinded");
	InsertMethodHash("OnEnteredSpit");
	InsertMethodHash("OnHitByVomitJar");
	InsertMethodHash("OnCommandAttack");
	InsertMethodHash("OnCommandAssault");
	InsertMethodHash("OnCommandRetreat");
	InsertMethodHash("OnCommandPause");
	InsertMethodHash("OnCommandResume");
	InsertMethodHash("OnCommandString");

	InsertMethodHash("IsAbleToBlockMovementOf");
	InsertMethodHash("ShouldPickUp");
	InsertMethodHash("ShouldHurry");
	InsertMethodHash("IsHindrance");
	InsertMethodHash("SelectTargetPoint");
	InsertMethodHash("IsPositionAllowed");
	InsertMethodHash("QueryCurrentPath");
	InsertMethodHash("SelectMoreDangerousThreat");

	InsertMethodHash("OnActorEmoted");
	InsertMethodHash("OnTerritoryContested");
	InsertMethodHash("OnTerritoryCaptured");
	InsertMethodHash("OnTerritoryLost");
	InsertMethodHash("OnWeaponFired");
	InsertMethodHash("OnWin");

	InsertMethodHash("ShouldRetreat");
	InsertMethodHash("ShouldAttack");

	InsertMethodHash("OnCommandApproach", "ByEntity", InsertMethodHashMode::Overload);
	InsertMethodHash("OnCommandApproach", "ByVector", InsertMethodHashMode::Overload);

	// Backwards compatibility
	InsertMethodHash("Update", "OnUpdate", InsertMethodHashMode::Alias);
	InsertMethodHash("InitialContainedAction", "OnInitialContainedAction", InsertMethodHashMode::Alias);
}

std::string ActionPublicsManager::BuildHashString(
	std::string_view prefix,
	const char* method,
	const char* value,
	InsertMethodHashMode mode,
	const char* aliasPart)
{
	std::string result(prefix);

	switch (mode)
	{
	case InsertMethodHashMode::Normal:
		result += method;
		break;

	case InsertMethodHashMode::Overload:
		result += method;
		result += value;
		break;

	case InsertMethodHashMode::Alias:
		result += aliasPart;
		break;

	default:
		throw std::invalid_argument("Invalid InsertMethodHashMode");
	}

	return result;
}