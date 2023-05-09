#pragma once

#ifndef _INCLUDE_HOOK_H
#define _INCLUDE_HOOK_H

class INextBot* GetEntityNextbotPointer(class CBaseEntity* entity);

bool CreateActionsHook();
void DestroyActionsHook();

#endif // !_INCLUDE_HOOK_H
