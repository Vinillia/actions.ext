#include <CDetour/detours.h>
#include "actionsdefs.h"
#include "actions_manager.h"
#include "actions_component.h"
#include "actions_processor.h"
#include "actions_tools.h"
#include "hook.h"

#include <NextBotInterface.h>
#include <NextBotComponentInterface.h>
#include <NextBotBehavior.h>
#include <NextBotIntentionInterface.h>

CDetour* g_pNextBotResetDetour = nullptr;

std::unordered_map<void*, int> g_vtable_hookids;

SH_DECL_HOOK0_void(IIntention, Reset, SH_NOATTRIB, 0);
void IIntention_ResetPost()
{
	IIntention* intention = META_IFACEPTR(IIntention);
	if (intention->GetBot())
	{
		g_pActionsTools->OnIntentionReset(intention->GetBot(), intention);
	}
}

DETOUR_DECL_MEMBER0(INextBot__Reset, void)
{
	INextBot* bot = (INextBot*)(this);

	DETOUR_MEMBER_CALL(INextBot__Reset)();

	//g_pActionsTools->OnNextBotReset(bot);

	for (INextBotComponent* component = bot->m_componentList; component != nullptr; component = component->m_nextComponent)
	{
		IIntention* intention = g_pActionsTools->TryCastToIntentionComponent(component);

		if (!intention)
			continue;

		g_pActionsTools->OnIntentionReset(bot, intention);

		void* vtable = *reinterpret_cast<void**>(intention);

		if (g_vtable_hookids.find(vtable) != g_vtable_hookids.end())
			continue;

		int hookid = SH_ADD_VPHOOK(IIntention, Reset, intention, IIntention_ResetPost, true);

		if (hookid == -1)
		{
			MsgSM("Failed to setup hook IIntention::Reset hook");
		}
		else
		{
			g_vtable_hookids[vtable] = hookid;
		}
	}
}

bool CreateActionsHook()
{
	g_pNextBotResetDetour = DETOUR_CREATE_MEMBER(INextBot__Reset, "INextBot::Reset");

	if (g_pNextBotResetDetour)
	{
		g_pNextBotResetDetour->EnableDetour();
		return true;
	}

	return false;
}

void DestroyActionsHook()
{
	if (g_pNextBotResetDetour)
	{
		g_pNextBotResetDetour->Destroy();
		g_pNextBotResetDetour = nullptr;
	}

	for (auto [key, value] : g_vtable_hookids)
	{
		SH_REMOVE_HOOK_ID(value);
	}

	g_vtable_hookids.clear();
}