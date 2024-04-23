#include "actions_constructor_smc.h"

StringHashMap<ActionConstructor_SMC::ac_data> m_acmap;

static bool IsPlatform(const char* plat)
{
	if (!strcmp(plat, "mac")		||
		!strcmp(plat, "osx")		||
		!strcmp(plat, "windows")	||
		!strcmp(plat, "win")		||
		!strcmp(plat, "linux")		||
		!strcmp(plat, "unix"))
	{
		return true;
	}

	return false;
}

static bool IsTargetPlatform(const char* plat)
{
#if defined WIN32
	return !strcmp(plat, "windows") || !strcmp(plat, "win");
#elif defined _LINUX
	return !strcmp(plat, "linux") || !strcmp(plat, "unix");
#endif
}

template<typename T>
static T complex(const char* value)
{
	/* 5000h */
	if (value[strlen(value) - 1] == 'h')
		return static_cast<T>(strtol(value, nullptr, 16));

	/* 0x5000 */
	if (value[0] == '0' && value[1] == 'x')
		return static_cast<T>(strtol(value, nullptr, 16));

	return static_cast<T>(atoi(value));
}

static bool StringToConvention(const char* value, CallConvention& convention)
{
	bool ok = false;

	if (!strcmp(value, "thiscall"))
	{
		convention = CallConv_ThisCall; ok = true;
	}
	else if (!strcmp(value, "cdecl"))
	{
		convention = CallConv_Cdecl; ok = true;
	}

	return ok;
}

static bool StringToPassType(const char* value, PassType& type)
{
	bool ok = false;

	if (!strcmp(value, "basic") || !strcmp(value, "plain"))
	{
		type = PassType_Basic; ok = true;
	}
	else if (!strcmp(value, "float"))
	{
		type = PassType_Float; ok = true;
	}
	else if (!strcmp(value, "object"))
	{
		type = PassType_Object; ok = true;
	}

	return ok;
}

static bool StringToFlags(const char* value, unsigned int& flags)
{
	std::string_view str(value);
	auto contains = [str](const char* substr) { return str.find(substr) != std::string::npos; };

	if (contains("byval"))
	{
		flags |= PASSFLAG_BYVAL;
	}
	else if (contains("byref"))
	{
		flags |= PASSFLAG_BYREF;
	}
	else if (contains("dtor") || contains("odtor"))
	{
		flags |= PASSFLAG_ODTOR;
	}
	else if (contains("ctor") || contains("octor"))
	{
		flags |= PASSFLAG_OCTOR;
	}
	else if (contains("assignop"))
	{
		flags |= PASSFLAG_OASSIGNOP;
	}
	else if (contains("unalign"))
	{
		flags |= PASSFLAG_OUNALIGN;
	}

	return true;
}

ActionConstructor_SMC::ActionConstructor_SMC()
{
	m_section[0] = '\0';
	m_state = SMC_Root;
	m_ignoreLevel = 0;
	m_currentLevel = 0;
	m_platformLevel = -1;
	m_data = ac_data();
}

void ActionConstructor_SMC::ReadSMC_ParseStart()
{
	m_state = SMC_Root;
	m_ignoreLevel = 0;
	m_currentLevel = 0;
	m_platformLevel = -1;
	m_data = ac_data();
}

SMCResult ActionConstructor_SMC::ReadSMC_NewSection(const SMCStates* states, const char* name)
{
	m_currentLevel++;

	if (m_ignoreLevel > 0)
	{
		m_ignoreLevel++;
		return SMCResult_Continue;
	}

	if (IsPlatform(name))
	{
		if (m_platformLevel != -1)
		{
			g_pSM->LogMessage(myself, "Nested platform section \"%s\". You have already specified OS. Line: %i, Col: %i", name, states->line, states->col);
			return SMCResult_HaltFail;
		}

		if (!IsTargetPlatform(name))
		{
			m_ignoreLevel++;
		}
		else
		{
			m_platformLevel = m_currentLevel;
		}

		return SMCResult_Continue;
	}

	if (m_state == SMC_Root)
	{
		m_state = SMC_Header;
		ke::SafeStrcpy(m_section, std::size(m_section), name);
	}
	else if (!strcmp(name, "args") || !strcmp(name, "arguments") ||
		!strcmp(name, "params") || !strcmp(name, "parameters"))
	{
		m_state = SMC_Params;
	}
	else if (m_state == SMC_Params)
	{
		if (m_data.param != nullptr)
		{
			g_pSM->LogMessage(myself, "Nested param section \"%s\". Params can't have params. Line: %i, Col: %i", name, states->line, states->col);
			return SMCResult_HaltFail;
		}

		m_data.param = &m_data.params.emplace_back();
	}

	return SMCResult_Continue;
}

SMCResult ActionConstructor_SMC::ReadSMC_KeyValue(const SMCStates* states, const char* key, const char* value)
{
	if (m_ignoreLevel > 0)
		return SMCResult_Continue;

	if (m_state == SMC_Header)
	{
		if (!strcmp(key, "size"))
		{
			m_data.size = complex<size_t>(value);
		}
		else if (!strcmp(key, "signature"))
		{
			ke::SafeStrcpy(m_data.signature, std::size(m_data.signature), value);
		}
		else if (!strcmp(key, "address"))
		{
			ke::SafeStrcpy(m_data.address, std::size(m_data.address), value);
		}
		else if (!strcmp(key, "callconv") || !strcmp(key, "conv") || !strcmp(key, "convention"))
		{
			if (!StringToConvention(value, m_data.convention))
			{
				g_pSM->LogMessage(myself, "Invalid call convention type: \"%s\".Line: % i, Col : % i", value, states->line, states->col);
				return SMCResult_HaltFail;
			}
		}
	}
	else if (m_state == SMC_Params)
	{
		if (m_data.param == nullptr)
			return SMCResult_HaltFail;

		if (!strcmp(key, "pass") || !strcmp(key, "type"))
		{
			if (!StringToPassType(value, m_data.param->info.type))
			{
				g_pSM->LogMessage(myself, "Invalid pass type: \"%s\".Line: % i, Col : % i", value, states->line, states->col);
				return SMCResult_HaltFail;
			}
		}
		else if (!strcmp(key, "flags"))
		{
			if (!StringToFlags(value, m_data.param->info.flags))
			{
				g_pSM->LogMessage(myself, "Invalid pass flags: \"%s\".Line: % i, Col : % i", value, states->line, states->col);
			}
		}
		else if (!strcmp(key, "encoder"))
		{
			ke::SafeStrcpy(m_data.param->encoder, std::size(m_data.param->encoder), value);
		}
	}

	return SMCResult_Continue;
}

SMCResult ActionConstructor_SMC::ReadSMC_LeavingSection(const SMCStates* states)
{
	m_currentLevel--;

	if (m_ignoreLevel > 0)
	{
		m_ignoreLevel--;
		return SMCResult_Continue;
	}

	if (m_platformLevel == m_currentLevel + 1)
	{
		m_platformLevel = -1;
		return SMCResult_Continue;
	}

	if (m_state == SMC_Params)
	{
		if (m_data.param == nullptr)
			m_state = SMC_Header;
		else
			m_data.param = nullptr;
	}
	else if (m_state == SMC_Header)
	{
		const ac_data* data = GetACData(m_section);

		//if (data == nullptr)
		{
			m_acmap.replace(m_section, m_data);
		}
		//else if (*data != m_data)
		//{
			//g_pSM->LogMessage(myself, "Duplicate \"%s\" constructor with different definition. Consider remove or rename.", m_section);
		//}

		m_state = SMC_Root;
		m_data = ac_data();
		m_ignoreLevel = 0;
		m_currentLevel = 0;
		m_platformLevel = -1;
	}

	return SMCResult_Continue;
}

const ActionConstructor_SMC::ac_data* ActionConstructor_SMC::GetACData(const char* name)
{
	auto r = m_acmap.find(name);

	if (r.found())
	{
		return &r->value;
	}

	return nullptr;
}