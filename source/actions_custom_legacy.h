#pragma once

#ifndef _INCLUDE_ACTIONS_CUSTOM_LEGACY_H
#define _INCLUDE_ACTIONS_CUSTOM_LEGACY_H

#include <NextBotBehavior.h>

class ActionCustomLegacy : private Action<CBaseEntity>
{
public:
	ActionCustomLegacy(const char* name);
	virtual const char* GetName() const override;

private:
	char m_sName[MAX_NAME_LENGTH];
};

#endif // !_INCLUDE_ACTIONS_CUSTOM_LEGACY_H
