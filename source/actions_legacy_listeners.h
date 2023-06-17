#pragma once

#define __LEGACY_NATIVE_MAKESTR(x) #x
#define __LEGACY_NATIVE_CONCAT(x,y) x##y

#define LEGACY_NATIVE_MAKESTR(x) __LEGACY_NATIVE_MAKESTR(x)
#define LEGACY_NATIVE_CONCAT(x,y) __LEGACY_NATIVE_CONCAT(x,y)

#define LEGACY_NATIVE_NAME(x) __LEGACY_NATIVE_MAKESTR(BehaviorAction.x.set)

#define REGISTER_LEGACY_NATIVE(name) \
do \
{ \
	LEGACY_NATIVE_CONCAT(LegacyNative,name) __legacy_native; \
} while(0) 

#define IMPLEMENT_LEGACY_NATIVE_OVERLOAD(name, pvarname)																		\
class LEGACY_NATIVE_CONCAT(LegacyNative,name)																							\
{																														\
	static inline constexpr std::string_view pvarstr = LEGACY_NATIVE_MAKESTR(pvarname);												\
	static inline constexpr std::string_view method = compile::concat<ActionPublicsManager::method_prefix, pvarstr>;	\
public:																													\
	static cell_t NAT_LegacyNative(IPluginContext* ctx, const cell_t* _params)											\
	{																													\
		cell_t params[5];																								\
		params[0] = _params[0];																							\
		params[1] = _params[1];																							\
		params[2] = compile::hash(method);																				\
		params[3] = _params[2];																							\
		params[4] = false;																								\
		return NAT_actions_setlistener(ctx, params);																	\
	}																													\
																														\
	static cell_t NAT_LegacyNativePost(IPluginContext* ctx, const cell_t* _params)										\
	{																													\
		cell_t params[5];																								\
		params[0] = _params[0];																							\
		params[1] = _params[1];																							\
		params[2] = compile::hash(method);																				\
		params[3] = _params[2];																							\
		params[4] = false;																								\
		return NAT_actions_setlistener(ctx, params);																	\
	}																													\
																														\
	LEGACY_NATIVE_CONCAT(LegacyNative,name)()																							\
	{																													\
		static sp_nativeinfo_t legacy_native[] =																		\
		{																												\
			{ LEGACY_NATIVE_NAME(name), NAT_LegacyNative },																\
			{ LEGACY_NATIVE_NAME(LEGACY_NATIVE_CONCAT(name,Post)), NAT_LegacyNativePost },											\
			{ NULL, NULL }																								\
		};																												\
																														\
		sharesys->AddNatives(myself, legacy_native);																	\
	}																													\
}

#define IMPLEMENT_LEGACY_NATIVE(name) IMPLEMENT_LEGACY_NATIVE_OVERLOAD(name, name)															

IMPLEMENT_LEGACY_NATIVE(OnStart);
IMPLEMENT_LEGACY_NATIVE(OnEnd);
IMPLEMENT_LEGACY_NATIVE(OnSuspend);
IMPLEMENT_LEGACY_NATIVE(OnResume);
IMPLEMENT_LEGACY_NATIVE(OnLeaveGround);
IMPLEMENT_LEGACY_NATIVE(OnLandOnGround);
IMPLEMENT_LEGACY_NATIVE(OnContact);
IMPLEMENT_LEGACY_NATIVE(OnMoveToSuccess);
IMPLEMENT_LEGACY_NATIVE(OnMoveToFailure);
IMPLEMENT_LEGACY_NATIVE(OnStuck);
IMPLEMENT_LEGACY_NATIVE(OnUnStuck);
IMPLEMENT_LEGACY_NATIVE(OnPostureChanged);
IMPLEMENT_LEGACY_NATIVE(OnAnimationActivityComplete);
IMPLEMENT_LEGACY_NATIVE(OnAnimationActivityInterrupted);
IMPLEMENT_LEGACY_NATIVE(OnAnimationEvent);
IMPLEMENT_LEGACY_NATIVE(OnIgnite);
IMPLEMENT_LEGACY_NATIVE(OnInjured);
IMPLEMENT_LEGACY_NATIVE(OnKilled);
IMPLEMENT_LEGACY_NATIVE(OnOtherKilled);
IMPLEMENT_LEGACY_NATIVE(OnSight);
IMPLEMENT_LEGACY_NATIVE(OnLostSight);
IMPLEMENT_LEGACY_NATIVE(OnThreatChanged);
IMPLEMENT_LEGACY_NATIVE(OnSound);
IMPLEMENT_LEGACY_NATIVE(OnSpokeConcept);
IMPLEMENT_LEGACY_NATIVE(OnNavAreaChanged);
IMPLEMENT_LEGACY_NATIVE(OnModelChanged);
IMPLEMENT_LEGACY_NATIVE(OnPickUp);
IMPLEMENT_LEGACY_NATIVE(OnShoved);
IMPLEMENT_LEGACY_NATIVE(OnBlinded);
IMPLEMENT_LEGACY_NATIVE(OnEnteredSpit);
IMPLEMENT_LEGACY_NATIVE(OnHitByVomitJar);
IMPLEMENT_LEGACY_NATIVE(OnCommandAttack);
IMPLEMENT_LEGACY_NATIVE(OnCommandAssault);
IMPLEMENT_LEGACY_NATIVE(OnCommandRetreat);
IMPLEMENT_LEGACY_NATIVE(OnCommandPause);
IMPLEMENT_LEGACY_NATIVE(OnCommandResume);
IMPLEMENT_LEGACY_NATIVE(OnCommandString);
IMPLEMENT_LEGACY_NATIVE(IsAbleToBlockMovementOf);
IMPLEMENT_LEGACY_NATIVE(ShouldPickUp);
IMPLEMENT_LEGACY_NATIVE(ShouldHurry);
IMPLEMENT_LEGACY_NATIVE(IsHindrance);
IMPLEMENT_LEGACY_NATIVE(SelectTargetPoint);
IMPLEMENT_LEGACY_NATIVE(IsPositionAllowed);
IMPLEMENT_LEGACY_NATIVE(QueryCurrentPath);
IMPLEMENT_LEGACY_NATIVE(SelectMoreDangerousThreat);
IMPLEMENT_LEGACY_NATIVE(OnTerritoryCaptured);
IMPLEMENT_LEGACY_NATIVE(OnTerritoryLost);
IMPLEMENT_LEGACY_NATIVE(OnWin);
IMPLEMENT_LEGACY_NATIVE(OnWeaponFired);
IMPLEMENT_LEGACY_NATIVE(OnActorEmoted);
IMPLEMENT_LEGACY_NATIVE(ShouldRetreat);
IMPLEMENT_LEGACY_NATIVE(ShouldAttack);

IMPLEMENT_LEGACY_NATIVE_OVERLOAD(OnCommandApproachV, OnCommandApproachByVector);
IMPLEMENT_LEGACY_NATIVE_OVERLOAD(OnCommandApproachE, OnCommandApproachByEntity);
IMPLEMENT_LEGACY_NATIVE_OVERLOAD(OnInitialContainedAction, InitialContainedAction);
IMPLEMENT_LEGACY_NATIVE_OVERLOAD(OnUpdate, Update);

void RegisterLegacyNatives()
{
	REGISTER_LEGACY_NATIVE(OnStart);
	REGISTER_LEGACY_NATIVE(OnUpdate);
	REGISTER_LEGACY_NATIVE(OnEnd);
	REGISTER_LEGACY_NATIVE(OnSuspend);
	REGISTER_LEGACY_NATIVE(OnResume);
	REGISTER_LEGACY_NATIVE(OnInitialContainedAction);
	REGISTER_LEGACY_NATIVE(OnLeaveGround);
	REGISTER_LEGACY_NATIVE(OnLandOnGround);
	REGISTER_LEGACY_NATIVE(OnContact);
	REGISTER_LEGACY_NATIVE(OnMoveToSuccess);
	REGISTER_LEGACY_NATIVE(OnMoveToFailure);
	REGISTER_LEGACY_NATIVE(OnStuck);
	REGISTER_LEGACY_NATIVE(OnUnStuck);
	REGISTER_LEGACY_NATIVE(OnPostureChanged);
	REGISTER_LEGACY_NATIVE(OnAnimationActivityComplete);
	REGISTER_LEGACY_NATIVE(OnAnimationActivityInterrupted);
	REGISTER_LEGACY_NATIVE(OnAnimationEvent);
	REGISTER_LEGACY_NATIVE(OnIgnite);
	REGISTER_LEGACY_NATIVE(OnInjured);
	REGISTER_LEGACY_NATIVE(OnKilled);
	REGISTER_LEGACY_NATIVE(OnOtherKilled);
	REGISTER_LEGACY_NATIVE(OnSight);
	REGISTER_LEGACY_NATIVE(OnLostSight);
	REGISTER_LEGACY_NATIVE(OnThreatChanged);
	REGISTER_LEGACY_NATIVE(OnSound);
	REGISTER_LEGACY_NATIVE(OnSpokeConcept);
	REGISTER_LEGACY_NATIVE(OnNavAreaChanged);
	REGISTER_LEGACY_NATIVE(OnModelChanged);
	REGISTER_LEGACY_NATIVE(OnPickUp);
	REGISTER_LEGACY_NATIVE(OnShoved);
	REGISTER_LEGACY_NATIVE(OnBlinded);
	REGISTER_LEGACY_NATIVE(OnEnteredSpit);
	REGISTER_LEGACY_NATIVE(OnHitByVomitJar);
	REGISTER_LEGACY_NATIVE(OnCommandAttack);
	REGISTER_LEGACY_NATIVE(OnCommandAssault);
	REGISTER_LEGACY_NATIVE(OnCommandApproachV);
	REGISTER_LEGACY_NATIVE(OnCommandApproachE);
	REGISTER_LEGACY_NATIVE(OnCommandRetreat);
	REGISTER_LEGACY_NATIVE(OnCommandPause);
	REGISTER_LEGACY_NATIVE(OnCommandResume);
	REGISTER_LEGACY_NATIVE(OnCommandString);
	REGISTER_LEGACY_NATIVE(IsAbleToBlockMovementOf);
	REGISTER_LEGACY_NATIVE(ShouldPickUp);
	REGISTER_LEGACY_NATIVE(ShouldHurry);
	REGISTER_LEGACY_NATIVE(IsHindrance);
	REGISTER_LEGACY_NATIVE(SelectTargetPoint);
	REGISTER_LEGACY_NATIVE(IsPositionAllowed);
	REGISTER_LEGACY_NATIVE(QueryCurrentPath);
	REGISTER_LEGACY_NATIVE(SelectMoreDangerousThreat);
	REGISTER_LEGACY_NATIVE(OnTerritoryCaptured);
	REGISTER_LEGACY_NATIVE(OnTerritoryLost);
	REGISTER_LEGACY_NATIVE(OnWin);
	REGISTER_LEGACY_NATIVE(OnWeaponFired);
	REGISTER_LEGACY_NATIVE(OnActorEmoted);
	REGISTER_LEGACY_NATIVE(ShouldRetreat);
	REGISTER_LEGACY_NATIVE(ShouldAttack);
}