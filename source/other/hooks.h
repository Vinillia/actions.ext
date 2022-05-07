#pragma once

#include "NextBotEventResponderInterface.h"
#include "NextBotInterface.h"

class NextBotIntention : public IIntention
{
public:
	CBaseEntity* entity;
	Behavior<void>* behavior;
	Behavior<void>* subehavior;

	Action<void>* GetAction() { return static_cast<Action<void>*>(behavior->FirstContainedResponder()); }
	Action<void>* GetSubAction() { return static_cast<Action<void>*>(subehavior->FirstContainedResponder()); }
};

SH_DECL_MANUALHOOK0_void(OnIntentionReset, INTENTION_RESET_OFFSET, 0, 0);

void OnIntentionReset()
{
	NextBotIntention* pIntention = META_IFACEPTR(NextBotIntention);
	ActionProcessor processor(pIntention->entity, pIntention->GetAction());
}

void OnSurviovrIntentionReset()
{
	NextBotIntention* pIntention = META_IFACEPTR(NextBotIntention);

	Action<void>* action = reinterpret_cast<Action<void>*>(pIntention->behavior->FirstContainedResponder());
	Action<void>* subaction = reinterpret_cast<Action<void>*>(pIntention->subehavior->FirstContainedResponder());

	ActionProcessor processor(pIntention->entity, action);
	ActionProcessor subprocessor(pIntention->entity, subaction);
}

void HookIntention(IGameConfig* cfg, const char* name, fastdelegate::FastDelegate<void> fastDelegate = SH_STATIC(OnIntentionReset))
{
	void* addr = NULL;

	if (!cfg->GetAddress(name, &addr))
	{
		LOGERROR("Failed to find address for \"%s\" key. Check your gamedata...", name);
		return;
	}
	
	SH_ADD_MANUALDVPHOOK(OnIntentionReset, addr, fastDelegate, true);
}

void CreateHooks(IGameConfig* config)
{
	HookIntention(config, "SurvivorIntention::Reset", SH_STATIC(OnSurviovrIntentionReset));
	HookIntention(config, "HunterIntention::Reset");
	HookIntention(config, "BoomerIntention::Reset");
	HookIntention(config, "TankIntention::Reset");
	HookIntention(config, "InfectedIntention::Reset");
	HookIntention(config, "WitchIntention::Reset");
	HookIntention(config, "SmokerIntention::Reset");

	#if SOURCE_ENGINE == SE_LEFT4DEAD2
		HookIntention(config, "ChargerIntention::Reset");
		HookIntention(config, "JockeyIntention::Reset");
		HookIntention(config, "SpitterIntention::Reset");
	#endif
}