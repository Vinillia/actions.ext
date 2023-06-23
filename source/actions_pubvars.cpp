#include "actions_pubvars.h"
#include "actions_processor.h"

#include "smsdk_ext.h"
#include "actions_encoders.h"

#define _MAKESTR(x) #x
#define MAKESTR(x) _MAKESTR(x)

#define INSERT_METHOD_HASH(method) \
do \
{	\
	static constexpr std::string_view _method = MAKESTR(method); \
	static constexpr auto hash = compile::concat<pubvar_prefix, _method>; \
	static constexpr std::string_view fn = compile::concat<method_prefix, _method>; \
	g_publicsManager.AddHash(hash.data(), compile::hash(fn)); \
} while(0)


#define INSERT_METHOD_HASH_OVERLOAD(method, overload, type) \
do \
{	\
	static constexpr std::string_view _method = MAKESTR(method); \
	static constexpr std::string_view _overload = MAKESTR(overload); \
	static constexpr auto hash = compile::concat<pubvar_prefix, _method, _overload>; \
	static constexpr auto fn = compile::concat<method_prefix, _method, _overload>; \
	g_publicsManager.AddHash(hash.data(), compile::hash(fn)); \
} while(0)


#define INSERT_METHOD_HASH_NAME(method, name, force) \
do \
{	\
	static constexpr std::string_view _method = MAKESTR(method); \
	static constexpr std::string_view _name = MAKESTR(name); \
	static constexpr auto value = compile::concat<pubvar_prefix, _name>; \
	g_publicsManager.AddHash(value.data(), compile::hash(method_prefix), force); \
} while(0)

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
		SetPluginPubVar(pl, iter->key.data(), (void*)iter->value);
		iter.next();
	}
}

void ActionPublicsManager::SyncEncoders(SourcePawn::IPluginContext* pl)
{
	auto encoders = ActionEncoder::GetActionEncoders();

	for (auto it = encoders->cbegin(); it != encoders->cend(); it++)
	{
		const ActionEncoder* encoder = *it;
		SetPluginPubVar(pl, encoder->PublicName(), (void*)encoder);
	}
}

bool ActionPublicsManager::SetPluginPubVar(SourcePawn::IPluginContext* pl, const char* name, void* value)
{
	uint32_t index;
	int err = pl->GetRuntime()->FindPubvarByName(name, &index);
	if (!err)
	{
		sp_pubvar_t* var;
		pl->GetRuntime()->GetPubvarByIndex(index, &var);
		if (var)
		{
			*var->offs = (cell_t)value;
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

void ActionPublicsManager::InitializePublicVariables()
{
	INSERT_METHOD_HASH(OnStart);
	INSERT_METHOD_HASH(Update);
	INSERT_METHOD_HASH(OnEnd);
	INSERT_METHOD_HASH(OnSuspend);
	INSERT_METHOD_HASH(OnResume);
	
	INSERT_METHOD_HASH(InitialContainedAction);
	
	INSERT_METHOD_HASH(OnLeaveGround);
	INSERT_METHOD_HASH(OnLandOnGround);
	INSERT_METHOD_HASH(OnContact);
	INSERT_METHOD_HASH(OnMoveToSuccess);
	INSERT_METHOD_HASH(OnMoveToFailure);
	INSERT_METHOD_HASH(OnStuck);
	INSERT_METHOD_HASH(OnUnStuck);
	INSERT_METHOD_HASH(OnPostureChanged);
	INSERT_METHOD_HASH(OnAnimationActivityComplete);
	INSERT_METHOD_HASH(OnAnimationActivityInterrupted);
	INSERT_METHOD_HASH(OnAnimationEvent);
	INSERT_METHOD_HASH(OnIgnite);
	INSERT_METHOD_HASH(OnInjured);
	INSERT_METHOD_HASH(OnKilled);
	INSERT_METHOD_HASH(OnOtherKilled);
	INSERT_METHOD_HASH(OnSight);
	INSERT_METHOD_HASH(OnLostSight);
	INSERT_METHOD_HASH(OnThreatChanged);
	INSERT_METHOD_HASH(OnSound);
	INSERT_METHOD_HASH(OnSpokeConcept);
	INSERT_METHOD_HASH(OnNavAreaChanged);
	INSERT_METHOD_HASH(OnModelChanged);
	INSERT_METHOD_HASH(OnPickUp);
	INSERT_METHOD_HASH(OnDrop);
	INSERT_METHOD_HASH(OnShoved);
	INSERT_METHOD_HASH(OnBlinded);
	INSERT_METHOD_HASH(OnEnteredSpit);
	INSERT_METHOD_HASH(OnHitByVomitJar);
	INSERT_METHOD_HASH(OnCommandAttack);
	INSERT_METHOD_HASH(OnCommandAssault);
	INSERT_METHOD_HASH(OnCommandRetreat);
	INSERT_METHOD_HASH(OnCommandPause);
	INSERT_METHOD_HASH(OnCommandResume);
	INSERT_METHOD_HASH(OnCommandString);

	INSERT_METHOD_HASH(IsAbleToBlockMovementOf);
	INSERT_METHOD_HASH(ShouldPickUp);
	INSERT_METHOD_HASH(ShouldHurry);
	INSERT_METHOD_HASH(IsHindrance);
	INSERT_METHOD_HASH(SelectTargetPoint);
	INSERT_METHOD_HASH(IsPositionAllowed);
	INSERT_METHOD_HASH(QueryCurrentPath);
	INSERT_METHOD_HASH(SelectMoreDangerousThreat);
	
	INSERT_METHOD_HASH_OVERLOAD(OnCommandApproach, ByEntity, EventDesiredResult< CBaseEntity >(ActionProcessor::*)(CBaseEntity*, const Vector&, float));
	INSERT_METHOD_HASH_OVERLOAD(OnCommandApproach, ByVector, EventDesiredResult< CBaseEntity >(ActionProcessor::*)(CBaseEntity*, CBaseEntity*));
	
	// Backwards compatibility
	INSERT_METHOD_HASH_NAME(Update, OnUpdate, true);
	INSERT_METHOD_HASH_NAME(InitialContainedAction, OnInitialContainedAction, true);
}