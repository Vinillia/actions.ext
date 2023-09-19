#pragma once

#ifndef _INCLUDE_ACTIONS_TOOLS_L4D2_H
#define _INCLUDE_ACTIONS_TOOLS_L4D2_H

#include "../../actions_tools.h"

class Intention;
class INextBot;

class L4D2ActionsTools : public ActionsTools
{
public:
	L4D2ActionsTools();
	
	virtual bool OnIntentionReset(INextBot* bot, IIntention* intention) override;
	virtual bool LoadGameConfigFile(SourceMod::IGameConfig* config, char* error, size_t maxlen) override;
	virtual void GetIntentionActions(IIntention* intention, std::vector<nb_action_ptr>& vec) override;

	nb_action_ptr ActionNextContainedResponder(IIntention* intention);
	inline CBaseEntity* MySurvivorBotPointer(INextBot* bot);

protected:
	int m_survivorBotPointerOffs;
};

inline CBaseEntity* L4D2ActionsTools::MySurvivorBotPointer(INextBot* bot)
{
	return ine::call_vtable<CBaseEntity*>(bot, m_survivorBotPointerOffs);
}

#endif // !_INCLUDE_ACTIONS_TOOLS_L4D2_H
