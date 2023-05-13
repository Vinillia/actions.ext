#ifndef _INCLUDE_HOOK_H
#define _INCLUDE_HOOK_H

#include <list>

class INextBot;

using ActionTree = std::list<nb_action_ptr>;

INextBot* GetEntityNextbotPointer(CBaseEntity* entity);
bool GetEntityActions(CBaseEntity* entity, ActionTree& tree);

bool CreateActionsHook();
void DestroyActionsHook();

#endif // !_INCLUDE_HOOK_H
