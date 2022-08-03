#pragma once

#include "NextBotEventResponderInterface.h"
#include "NextBotInterface.h"

extern void ExecuteProcessor(CBaseEntity* entity, Action<void>* action);
SH_DECL_MANUALHOOK0_void(OnIntentionReset, 0, 0, 0);

class NextBotIntention : public IIntention
{
public:
	CBaseEntity* entity;
	Behavior<void>* behavior;
	Behavior<void>* subehavior;

	Action<void>* GetAction() { return static_cast<Action<void>*>(behavior->FirstContainedResponder()); }
	Action<void>* GetSubAction() { return static_cast<Action<void>*>(subehavior->FirstContainedResponder()); }
};

void OnIntentionReset()
{
	NextBotIntention* pIntention = META_IFACEPTR(NextBotIntention);
	ExecuteProcessor(pIntention->entity, pIntention->GetAction());
}

void OnSurviovrIntentionReset()
{
	NextBotIntention* pIntention = META_IFACEPTR(NextBotIntention);

	ExecuteProcessor(pIntention->entity, pIntention->GetAction());
	ExecuteProcessor(pIntention->entity, pIntention->GetSubAction());
}

void HookIntention(const char* name, fastdelegate::FastDelegate<void> fastDelegate = SH_STATIC(OnIntentionReset))
{
	void* addr = static_cast<void*>(GetOffsetsManager()->RequestAddress(name));

	if (addr == nullptr)
	{
		LOGERROR("Failed to find address for \"%s\" key. Check your gamedata...", name);
		return;
	}
	
	SH_ADD_MANUALDVPHOOK(OnIntentionReset, addr, fastDelegate, true);
}

void CreateHooks()
{
	int32_t intention_reset = GetOffsetsManager()->RequestOffset("IIntention::Reset");

	if (GetOffsetsManager()->HaveFailedRequest())
		return;

	SH_MANUALHOOK_RECONFIGURE(OnIntentionReset, intention_reset, 0, 0);

	HookIntention("SurvivorIntention::Reset", SH_STATIC(OnSurviovrIntentionReset));
	HookIntention("HunterIntention::Reset");
	HookIntention("BoomerIntention::Reset");
	HookIntention("TankIntention::Reset");
	HookIntention("InfectedIntention::Reset");
	HookIntention("WitchIntention::Reset");
	HookIntention("SmokerIntention::Reset");
}