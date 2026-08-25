
#include "extension.h"
#include "actions/runtime/tools.h"

#include "NextBotInterface.h"

const char* INextBot::GetDebugIdentifier(void) const
{
	const int nameSize = 256;
	static char name[nameSize];

	CBaseEntity* entity = g_pActionsTools->GetEntity(const_cast<INextBot*>(this));
	int entindex = gamehelpers->EntityToBCompatRef(entity);
    const char* classname = gamehelpers->GetEntityClassname(entity);

	Q_snprintf(name, nameSize, "%s(#%d)", classname ? classname : "unknown", entindex);

	return name;
}
