#pragma once

#ifndef _INCLUDE_ACTIONS_COMMANDS_H
#define _INCLUDE_ACTIONS_COMMANDS_H

#include <iplayerinfo.h>
#include "offset_manager.h"

enum
{
    DUMP_FLAG_CLIENTS   = 0x01,
    DUMP_FLAG_INFECTED  = 0x02,
    DUMP_FLAG_WITCHES   = 0x04,
    DUMP_FLAG_ALL       = 0xFFFFFFFF
};

inline bool ClassMatchesComplex(cell_t entity, const char* match);

CON_COMMAND(ext_actions_dump, "Dump entities actions")
{
    LOG("DUPMING ACTIONS START");
    LOG("/----------------------------------/");

    std::vector<Action<void>*> actions;
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

            size_t num = g_pActionsManager->GetEntityActions(entity, &actions);

            if (!num)
                return;

            LOG("BOT %s has %i actions:", player->GetName(), num);

            for(size_t i = 0; i < num; i++)
            {
                LOG("%i. %s %s %s ( %X ) ", i + 1, actions.at(i)->GetName(), actions.at(i)->m_isStarted ? "STARTED" : "NOT STARTED", 
                actions.at(i)->IsSuspended() ? "SUSPENDED" : "",
                actions.at(i));
            }
        }
        else
        {
            CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);
            
            if (!pEntity)
                return;

            size_t num = g_pActionsManager->GetEntityActions(entity, &actions);

            if (!num)
                return;

            LOG("ENTITY %s(%i) has %i actions:", gamehelpers->GetEntityClassname(pEntity), entity, num);

            for(size_t i = 0; i < num; i++)
            {
                LOG("%i. %s %s %s ( %X ) ", i + 1, actions.at(i)->GetName(), actions.at(i)->m_isStarted ? "STARTED" : "NOT STARTED", 
                actions.at(i)->IsSuspended() ? "SUSPENDED" : "",
                actions.at(i));
            }
        }
        
        LOG("/----------------------------------/");
    };

    int clients = playerhelpers->GetMaxClients();
    for(cell_t entity = 1; entity <= 2048; entity++)
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

    LOG("DUPMING ACTIONS END");
}

CON_COMMAND(ext_actions_offsets, "Dump extension functions offset")
{
    #ifndef __linux__ 
    static const char* OS = "Windows";
    #else
    static const char* OS = "Linux";
    #endif

    LOG("OS: %s | Game: %s", OS, g_pSM->GetGameFolderName());
    GetOffsetsManager()->Dump();
}

CON_COMMAND(ext_actions_listeners, "Dump actions listeners")
{
    if (args.ArgC() > 1)
    {
        g_pActionsPropagatePost->Dump();
    }
    else
    {
        g_pActionsPropagatePre->Dump();
    }
}

CON_COMMAND(ext_actions_list, "Dump every action that manager currently holds")
{
    LOG("/----------------------------------/");
    g_pActionsManager->Dump();
    LOG("/----------------------------------/");
}

inline bool ClassMatchesComplex(cell_t entity, const char* match)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);

    if (!pEntity)
        return false;

    return strcmp(gamehelpers->GetEntityClassname(pEntity), match) == 0;
}

#endif // _INCLUDE_ACTIONS_COMMANDS_H