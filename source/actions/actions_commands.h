#pragma once

#include <iplayerinfo.h>

enum
{
    DUMP_FLAG_CLIENTS   = 0x01,
    DUMP_FLAG_INFECTED  = 0x02,
    DUMP_FLAG_WITCHES   = 0x04,
    DUMP_FLAG_ALL       = 0xFFFFFFFF
};

inline bool ClassMatchesComplex(cell_t entity, const char* match);

CON_COMMAND(ext_actions_dump, "Dumps all actions")
{
    LOG("DUPMING ACTIONS START");

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

            LOG("/----------------------------------/");
            LOG("BOT %s has %i actions:", player->GetName(), num);

            for(size_t i = 0; i < num; i++)
            {
                LOG("%i. %s ( %X )", i + 1, actions.at(i)->GetName(), actions.at(i));
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

            LOG("/----------------------------------/");
            LOG("ENTITY %s(%i) has %i actions:", gamehelpers->GetEntityClassname(pEntity), entity, num);

            for(size_t i = 0; i < num; i++)
            {
                LOG("%i. %s ( %X )", i + 1, actions.at(i)->GetName(), actions.at(i));
            }
        }
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

inline bool ClassMatchesComplex(cell_t entity, const char* match)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(entity);

    if (!pEntity)
        return false;

    return strcmp(gamehelpers->GetEntityClassname(pEntity), match) == 0;
}