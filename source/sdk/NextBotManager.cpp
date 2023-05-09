#include "extension.h"

#include "NextBotManager.h"
#include "NextBotInterface.h"

NextBotManager& TheNextBots(void* pfn)
{
	static auto fnTheNextBots = reinterpret_cast<NextBotManager&(__cdecl*)()>(pfn);
	return fnTheNextBots();
}

bool NextBotManager::IsDebugFilterMatch(const INextBot* bot) const
{
	if (!g_sdkActions.IsNextBotDebugSupported())
		return false;

	// if the filter is empty, all bots match
	if (m_debugFilterList.Count() == 0)
	{
		return true;
	}

	for (int i = 0; i < m_debugFilterList.Count(); ++i)
	{
		// compare entity index
		if (m_debugFilterList[i].index == gamehelpers->EntityToReference((CBaseEntity*)const_cast<INextBot*>(bot)->GetEntity()))
		{
			return true;
		}

		// compare debug filter
		if (m_debugFilterList[i].name[0] != '\000' && bot->IsDebugFilterMatch(m_debugFilterList[i].name))
		{
			return true;
		}

		// compare special keyword meaning local player is looking at them
		if (!Q_strnicmp(m_debugFilterList[i].name, "lookat", Q_strlen(m_debugFilterList[i].name)))
		{
			/*
			CBasePlayer* watcher = gamehelpers->ReferenceToEntity(1);
			if (watcher)
			{
				CBaseEntity* subject = watcher->GetObserverTarget();

				if (subject && bot->IsSelf(subject))
				{
					return true;
				}
			}
			*/
		}

		// compare special keyword meaning NextBot is selected
		if (!Q_strnicmp(m_debugFilterList[i].name, "selected", Q_strlen(m_debugFilterList[i].name)))
		{
			INextBot* selected = GetSelected();
			if (selected && bot->IsSelf((CBaseEntity*)selected->GetEntity()))
			{
				return true;
			}
		}
	}

	return false;
}
