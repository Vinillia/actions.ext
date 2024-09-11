#pragma once

#ifndef _INCLUDE_ACTIONS_TOOLS_L4D_H
#define _INCLUDE_ACTIONS_TOOLS_L4D_H

#include "../../actions_tools.h"
#include "../l4d2/actions_tools_l4d2.h"

class L4DActionTools : public L4D2ActionsTools
{
public:
#ifndef WIN32
	virtual IIntention* GetIntentionComponent(INextBotComponent* component) override;
#endif
};

#endif // !_INCLUDE_ACTIONS_TOOLS_L4D_H
