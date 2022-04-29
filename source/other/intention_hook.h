#pragma once

SH_DECL_MANUALHOOK0_void(OnIntentionReset, INTENTION_RESET_OFFSET, 0, 0);

class Intention : private IIntention
{
public:
	CBaseEntity* entity;
	Behavior<void>* behavior;
	Behavior<void>* subehavior;
};

void OnIntentionReset()
{
	Intention* intention = META_IFACEPTR(Intention);
	Action<void>* action = static_cast<Action<void>*>(intention->behavior->FirstContainedResponder());

	ActionProcessor processor(intention->entity, action);
}

void OnSurviovrIntentionReset()
{
	Intention* intention = META_IFACEPTR(Intention);

	Action<void>* action = static_cast<Action<void>*>(intention->behavior->FirstContainedResponder());
	Action<void>* subaction = static_cast<Action<void>*>(intention->subehavior->FirstContainedResponder());

	ActionProcessor processor(intention->entity, action);
	ActionProcessor subprocessor(intention->entity, subaction);
}

void HookIntention(IGameConfig* config, const char* name, fastdelegate::FastDelegate<void> fastDelegate = SH_STATIC(OnIntentionReset))
{
	void* addr = NULL, **pAddr = NULL;

	if (!config->GetMemSig(name, &addr))
	{
		LOGERROR("Failed to get \"%s\" key. Check your gamedata...", name);
		return;
	}
	else if (addr == NULL)
	{
		LOGERROR("Failed to find address for \"%s\". Update your gamedata", name);
		return;
	}

	pAddr = &addr;
	SH_ADD_MANUALVPHOOK(OnIntentionReset, pAddr, fastDelegate, true);
}

void HookIntentions(IGameConfig* config)
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