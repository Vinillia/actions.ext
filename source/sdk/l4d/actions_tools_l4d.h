#ifndef ACTIONS_TOOLS_L4D_H
#define ACTIONS_TOOLS_L4D_H

#include "../l4d2/actions_tools_l4d2.h"

class L4DActionsTools : public L4D2ActionsTools
{
public:
	virtual IIntention* TryCastToIntentionComponent(INextBotComponent* component) override;
};

#endif // !ACTIONS_TOOLS_L4D_H
