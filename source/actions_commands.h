#pragma once

#ifndef _INCLUDE_ACTIONS_COMMANDS_H
#define _INCLUDE_ACTIONS_COMMANDS_H

#include <iplayerinfo.h>
#include "hook.h"

inline bool ClassMatchesComplex(cell_t entity, const char* match);
inline bool ClassMatchesComplex(CBaseEntity* entity, const char* match);

CON_COMMAND(ext_actions_dump, "Dump entities actions")
{
    Msg("DUPMING ACTIONS START");
    Msg("/----------------------------------/");

    ActionTree tree;

    auto dump = [&tree](CBaseEntity* entity, const char* name) -> void
    {
        tree.clear();
        if (!GetEntityActions(entity, tree))
            return;

        Msg("%s has %i actions:", name, tree.size());
        int i = 0;
        for (auto action : tree)
        {
            Msg("%i. %s %s %s ( 0x%X ) ", ++i, action->GetName(), action->m_isStarted ? "STARTED" : "NOT STARTED",
                action->IsSuspended() ? "SUSPENDED" : "",
                action);
        }
    };

    int clients = playerhelpers->GetMaxClients();
    for (cell_t i = 1; i <= clients; i++)
    {
        IGamePlayer* player = playerhelpers->GetGamePlayer(i);

        if (!player->IsInGame() || !player->IsFakeClient())
            continue;

        CBaseEntity* entity = gamehelpers->ReferenceToEntity(i);

        if (entity != nullptr)
            dump(entity, player->GetName());
    }

    for (cell_t i = clients + 1; i <= 2048; i++)
    {
        CBaseEntity* entity = gamehelpers->ReferenceToEntity(i);

        if (entity == nullptr)
            continue;

        const char* name = gamehelpers->GetEntityClassname(entity);
        dump(entity, name);
    }

    Msg("DUPMING ACTIONS END");
}

inline bool ClassMatchesComplex(cell_t entity, const char* match)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);

	if (!pEntity)
		return false;

	return ClassMatchesComplex(entity, match);
}

inline bool ClassMatchesComplex(CBaseEntity* entity, const char* match)
{
    return strcmp(gamehelpers->GetEntityClassname(entity), match) == 0;
}

#endif // _INCLUDE_ACTIONS_COMMANDS_H