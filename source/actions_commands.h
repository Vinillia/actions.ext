#pragma once

#ifndef _INCLUDE_ACTIONS_COMMANDS_H
#define _INCLUDE_ACTIONS_COMMANDS_H

#include <iplayerinfo.h>

enum
{
    DUMP_FLAG_CLIENTS = 0x01,
    DUMP_FLAG_INFECTED = 0x02,
    DUMP_FLAG_WITCHES = 0x04,
    DUMP_FLAG_ALL = 0xFFFFFFFF
};

inline bool ClassMatchesComplex(cell_t entity, const char* match);

CON_COMMAND(ext_actions_dump, "Dump entities actions")
{
    Msg("DUPMING ACTIONS START");
    Msg("/----------------------------------/");

    std::vector<nb_action_ptr> actions;
    int flags = DUMP_FLAG_ALL;

    if (args.ArgC() > 1)
        flags = atoi(args[1]);

    auto dump = [&](cell_t entity) -> void
    {
        actions.clear();

        if (entity <= playerhelpers->GetMaxClients())
        {
            IGamePlayer* player = playerhelpers->GetGamePlayer(entity);

            if (!player || !player->IsInGame() || !player->IsFakeClient())
                return;

            g_actionsManager.GetEntityActions(gamehelpers->ReferenceToEntity(entity), actions);

            if (!actions.size())
                return;

            Msg("BOT %s has %i actions:", player->GetName(), actions.size());

            for (size_t i = 0; i < actions.size(); i++)
            {
                Msg("%i. %s %s %s ( %X ) ", i + 1, actions.at(i)->GetName(), actions.at(i)->m_isStarted ? "STARTED" : "NOT STARTED",
                    actions.at(i)->IsSuspended() ? "SUSPENDED" : "",
                    actions.at(i));
            }
        }
        else
        {
            CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);

            if (!pEntity)
                return;

            g_actionsManager.GetEntityActions(pEntity, actions);

            if (!actions.size())
                return;

            Msg("ENTITY %s(%i) has %i actions:", gamehelpers->GetEntityClassname(pEntity), entity, actions.size());

            for (size_t i = 0; i < actions.size(); i++)
            {
                Msg("%i. %s %s %s ( %X ) ", i + 1, actions.at(i)->GetName(), actions.at(i)->m_isStarted ? "STARTED" : "NOT STARTED",
                    actions.at(i)->IsSuspended() ? "SUSPENDED" : "",
                    actions.at(i));
            }
        }

        Msg("/----------------------------------/");
    };

    int clients = playerhelpers->GetMaxClients();
    for (cell_t entity = 1; entity <= 2048; entity++)
    {
        if (entity <= clients)
        {
            if (flags & DUMP_FLAG_CLIENTS)
                dump(entity);
        }
        else
        {
            if (flags & DUMP_FLAG_WITCHES && ClassMatchesComplex(entity, "witch"))
                dump(entity);

            if (flags & DUMP_FLAG_INFECTED && ClassMatchesComplex(entity, "infected"))
                dump(entity);
        }
    }

    Msg("DUPMING ACTIONS END");
}

inline bool ClassMatchesComplex(cell_t entity, const char* match)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);

    if (!pEntity)
        return false;

    return strcmp(gamehelpers->GetEntityClassname(pEntity), match) == 0;
}

#endif // _INCLUDE_ACTIONS_COMMANDS_H