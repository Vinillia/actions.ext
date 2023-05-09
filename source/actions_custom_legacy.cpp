
#include "am-string.h"
#include "actions_custom_legacy.h"

ActionCustomLegacy::ActionCustomLegacy(const char* name) : Action<CBaseEntity>()
{
	ke::SafeStrcpyN(m_sName, MAX_NAME_LENGTH, name, MAX_NAME_LENGTH);
}

const char* ActionCustomLegacy::GetName() const 
{ 
	return m_sName; 
}