#include "actionsdefs.h"
#include "actions_tools_l4d.h"
#include "../../l4d2/actions_tools_l4d2.cpp"

#ifndef WIN32
IIntention* L4DActionTools::GetIntentionComponent(INextBotComponent* component)
{
    const std::type_info& lhs = typeid(IIntention);
    const std::type_info& rhs = typeid(component);

    MsgSM("%s %s %i", typeid(component).name(), typeid(component).raw_name(), typeid(lhs).before(rhs));
    return nullptr;
}
#endif
