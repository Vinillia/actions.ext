#include"extension.h"
#include "actions_constructor.h"

#include <stdexcept>
#include <stack>
#include "NextBotBehavior.h"

static bool IsPlatform(const char* plat)
{
	if (!strcmp(plat, "mac") ||
		!strcmp(plat, "osx") ||
		!strcmp(plat, "windows") ||
		!strcmp(plat, "win") ||
		!strcmp(plat, "linux") ||
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
		return (T)strtol(value, nullptr, 16);

	/* 0x5000 */
	if (value[0] == '0' && value[1] == 'x')
		return (T)strtol(value, nullptr, 16);

	return (T)atoi(value);
}

static bool StringToPassType(const char* value, PassType& type)
{
	bool ok = false;

	if (!strcmp(value, "basic") || !strcmp(value, "plain"))
	{
		type = PassType_Basic; ok = true;
	}
	else if (!strcmp(value, "float") || !strcmp(value, "xmm"))
	{
		type = PassType_Float; ok = true;
	}
	else if (!strcmp(value, "float") || !strcmp(value, "xmm"))
	{
		type = PassType_Float; ok = true;
	}

	return ok;
}

static void StringToFlags(const char* value, unsigned int& flags)
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
}

ActionConstructor_SMC::ActionConstructor_SMC()
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
			Warning("Nested platform section \"%s\". You have already specified OS. Line: %i, Col: %i", name, states->line, states->col);
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
		m_section = name;
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
			Warning("Nested param section \"%s\". Params can't have params. Line: %i, Col: %i", name, states->line, states->col);
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
			m_data.signature = value;
		}
		else if (!strcmp(key, "address"))
		{
			m_data.address = value;
		}
	}
	else if (m_state == SMC_Params)
	{
		if (m_data.param == nullptr)
			return SMCResult_HaltFail;
			
		if (!strcmp(key, "pass") || !strcmp(key, "type"))
		{
			StringToPassType(value, m_data.param->info.type);
		}
		else if (!strcmp(key, "flags"))
		{
			StringToFlags(value, m_data.param->info.flags);
		}
		else if (!strcmp(key, "encoder"))
		{
			m_data.param->encoder = value;
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
		m_acmap.insert(m_section.c_str(), m_data);
		m_state = SMC_Root;
		m_data = ac_data();
		m_ignoreLevel = 0;
		m_currentLevel = 0;
		m_platformLevel = -1;
	}

	return SMCResult_Continue;
}

void ActionConstructor_SMC::ReadSMC_ParseStart()
{
	m_state = SMC_Root;
	m_ignoreLevel = 0;
	m_currentLevel = 0;
	m_platformLevel = -1;
	m_data = ac_data();
}

ActionConstructor::ActionConstructor(int actionSize)
{
	m_address = nullptr;
	m_call = nullptr;
	m_actionSize = 0;
	m_paramCount = 0;
	m_paramSize = 4;
}

ActionConstructor::~ActionConstructor()
{
	if (m_call)
		m_call->Destroy();
}

bool ActionConstructor::ConstructParamsBuffer(IPluginContext* ctx, const cell_t* params, cell_t* buffer, cell_t numParams)
{
	if (numParams - 1 > SP_MAX_EXEC_PARAMS)
	{
		ctx->ReportErrorNumber(SP_ERROR_PARAMS_MAX);
		return false;
	}

	cell_t* addr;
	for (int i = 2; i <= numParams; i++)
	{
		ctx->LocalToPhysAddr(params[i], &addr);
		//buffer[i - 2] = (m_param[i - 2].flags & PASSFLAG_BYREF ? (cell_t)addr : *addr);
		buffer[i - 2] = (cell_t)addr;
	}

	return true;
}

nb_action_ptr ActionConstructor::Execute(IPluginContext* ctx, const cell_t* params, cell_t numParams)
{
	if (m_address == nullptr)
		throw std::runtime_error("Invalid function address");

	if (m_call == nullptr)
		throw std::runtime_error("Call hasn't been finished yet");

	if (m_call->GetParamCount() + 1 != numParams)
		throw std::runtime_error("Parameters count doesn't match");

	if (m_actionSize <= sizeof(nb_action))
		throw std::runtime_error("Bad action size");

	cell_t paramsBuffer[SP_MAX_EXEC_PARAMS] = {};
	if (!ConstructParamsBuffer(ctx, params, paramsBuffer, numParams))
		throw std::runtime_error("Failed to construct params buffer");

	const int maxlength = 255;
	char error[maxlength] = {};

	stack vstk(m_paramSize + sizeof(nb_action_ptr));

	nb_action_ptr action = AllocateAction();
	vstk.put(action, sizeof(nb_action_ptr));

	for (int i = 0; i < numParams; i++, i++)
	{
		int size = m_param[i].size;
		void* param = (void*)paramsBuffer[i];

		const Encoder& encoder = *(m_paramEncoder[i]);
		if (!encoder(param, error, maxlength))
		{
			throw std::runtime_error(error);
		}

		vstk.put(param, size);
	}

	m_call->Execute(vstk.get(), nullptr);
	return action;
}

bool ActionConstructor::AddParameter(PassType type, int flags, const Encoder* encoder)
{
	if (m_paramCount >= SP_MAX_EXEC_PARAMS)
		throw std::runtime_error("Exceed SP_MAX_EXEC_PARAMS");

	PassInfo info;
	
	info.fields = nullptr;
	info.flags = flags;
	info.numFields = 0;
	info.type = type;

	if (type == PassType_Basic)
	{
		info.size = sizeof(cell_t);
	}
	else if (type == PassType_Float)
	{
		info.size = sizeof(float);
	}
	else if (type == PassType_Object)
	{
		if (encoder == nullptr)
		{
			throw std::runtime_error("Object type has no encoder");
		}
	}

	if (encoder == nullptr)
	{
		static Encoder defaultEncoder("g_defaultEncoder", "default");
		encoder = &defaultEncoder;
	}

	if (info.flags == 0)
		info.flags = PASSFLAG_BYVAL;

	const int maxlength = 255;
	char error[maxlength] = {};
	if (encoder && !(*encoder)(info, error, maxlength))
	{
		throw std::runtime_error(error);
		return false;
	}

	m_paramSize += info.size;
	m_param[m_paramCount] = info;
	m_paramEncoder[m_paramCount] = encoder;
	m_paramCount++;
	return true;
}

bool ActionConstructor::Finish()
{
	if (m_address == nullptr)
		return false;

	m_call = bintools->CreateCall(m_address, CallConv_ThisCall, nullptr, m_param, m_paramCount);
	return m_call != nullptr;
}

bool ActionConstructor::AddressFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor)
{
	if (!config->GetMemSig(constructor, &m_address) || m_address == nullptr)
	{
		if (!config->GetAddress(constructor, &m_address) || m_address == nullptr)
		{
			ctx->ReportError("Failed to set signature nor address (%s)", constructor);
			return false;
		}
	}

	return true;
}

bool ActionConstructor::SetupFromConf(IPluginContext* ctx, IGameConfig* config, const ac_data* data)
{
	if (data->signature.length() > 0)
	{
		if (!config->GetMemSig(data->signature.c_str(), &m_address) || m_address == nullptr)
		{
			ctx->ReportError("Failed to set signature (%s)", data->signature.c_str());
			return false;
		}
	}

	if (data->address.length() > 0)
	{
		if (!config->GetAddress(data->address.c_str(), &m_address) || m_address == nullptr)
		{
			ctx->ReportError("Failed to set address (%s)", data->address.c_str());
			return false;
		}
	}

	SetSize(data->size);

	auto& encoders = Encoder::GetEncoders();
	auto& vec = data->params;
	auto getEncoder = [&encoders](const param_t& param) -> const Encoder*
	{
		auto r = std::find_if(encoders.cbegin(), encoders.cend(), [&param](const Encoder* encoder)
			{
				return !strcmp(encoder->ShortName(), param.encoder.c_str()) || !strcmp(encoder->PublicName(), param.encoder.c_str());
			}
		);

		return (r == encoders.end() ? nullptr : *r);
	};

	for (auto it = vec.cbegin(); it != vec.cend(); it++)
	{
		const param_t& param = *it;
		const Encoder* encoder = getEncoder(param);

		if (param.encoder.length() > 0 && encoder == nullptr)
		{
			ctx->ReportError("Failed to find specified encoder (%s)", param.encoder.c_str());
			return false;
		}

		AddParameter(param.info.type, param.info.flags, encoder);
	}

	if (!Finish())
	{
		ctx->ReportError("Failed to finish constructor");
		return false;
	}

	return true;
}