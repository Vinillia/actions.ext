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
int g_iHookID = -1;

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

	for (INextBotComponent* component = bot->m_componentList; component != nullptr; component = component->m_nextComponent)
	{
		IIntention* intention = dynamic_cast<IIntention*>(component);

		if (intention == nullptr)
			continue;

		g_pActionsTools->OnIntentionReset(bot, intention);

		if (g_iHookID == -1)
		{
			g_iHookID = SH_ADD_VPHOOK(IIntention, Reset, intention, IIntention_ResetPost, true);

			if (g_iHookID == -1)
			{
				MsgSM("Failed to setup hook IIntention::Reset hook");
			}
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

	if (g_iHookID != -1)
	{
		SH_REMOVE_HOOK_ID(g_iHookID);
		g_iHookID = -1;
	}
}