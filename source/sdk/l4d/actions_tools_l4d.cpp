#include "actionsdefs.h"
#include "actions_tools_l4d.h"
#include "../../l4d2/actions_tools_l4d2.cpp"

L4DActionTools g_L4DActionsTools;

#ifndef WIN32
IIntention* L4DActionTools::GetIntentionComponent(INextBotComponent* component)
{
    const std::type_info& lhs = typeid(IIntention);
    const std::type_info& rhs = typeid(component);

    MsgSM("%s %i", typeid(component).name(), typeid(lhs).before(rhs));
    return nullptr;
}
#endif
