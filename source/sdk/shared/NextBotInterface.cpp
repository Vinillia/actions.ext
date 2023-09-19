
#include "extension.h"
#include "actions_tools.h"

#include "NextBotInterface.h"

const char* INextBot::GetDebugIdentifier(void) const
{
	const int nameSize = 256;
	static char name[nameSize];

	CBaseEntity* entity = g_pActionsTools->GetEntity(const_cast<INextBot*>(this));
	int entindex = gamehelpers->EntityToBCompatRef(entity);

	Q_snprintf(name, nameSize, "%s(#%d)", entity, entindex);

	return name;
}
