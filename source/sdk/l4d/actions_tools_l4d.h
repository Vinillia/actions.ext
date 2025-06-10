#pragma once

#ifndef _INCLUDE_ACTIONS_TOOLS_L4D_H
#define _INCLUDE_ACTIONS_TOOLS_L4D_H

#include "../l4d2/actions_tools_l4d2.h"

class L4DActionsTools : public L4D2ActionsTools
{
public:
	virtual IIntention* TryCastToIntentionComponent(INextBotComponent* component) override;
};

#endif // !_INCLUDE_ACTIONS_TOOLS_L4D_H
