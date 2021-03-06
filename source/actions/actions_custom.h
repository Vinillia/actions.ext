#pragma once

class PluginAction : public Action<void>
{
public:
	PluginAction(const char* name)
	{
		ke::SafeStrcpy(m_szName, sizeof(m_szName), name);
	}

	virtual const char* GetName(void) const override { return m_szName; }
private:
#define MAX_NAME_LENGTH 32
	char m_szName[MAX_NAME_LENGTH * 2];
};

