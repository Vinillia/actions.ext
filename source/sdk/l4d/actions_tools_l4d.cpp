#include "actionsdefs.h"
#include "actions_tools_l4d.h"
#include "NextBotComponentInterface.h"

#include <am-platform.h>

#include "../../l4d2/actions_tools_l4d2.cpp"

#if defined(KE_LINUX)
#include <cxxabi.h>
#endif

L4DActionsTools g_L4DActionsTools;

#if defined(KE_LINUX)
std::string Demangle(const char* mangled) 
{
    int status = 0;
    char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
    std::string result = (status == 0 && demangled) ? demangled : mangled;
    free(demangled);
    return result;
}

bool IsIIntention(INextBotComponent* i) 
{
    std::string name = Demangle(typeid(*i).name());
    return name.find("Intention") != std::string::npos;
}
#endif

IIntention* L4DActionsTools::TryCastToIntentionComponent(INextBotComponent* component)
{
#if defined(KE_LINUX)
	return IsIIntention(component) ? reinterpret_cast<IIntention*>(component) : nullptr;
#else
	return ActionsTools::TryCastToIntentionComponent(component);
#endif
}