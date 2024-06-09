#ifdef INCLUDE_ACTIONS_CONSTRUCTOR

#include "extension.h"
#include "actions_constructor.h"

#include <stdexcept>
#include <stack>
#include <cstring>

#include "NextBotBehavior.h"

std::vector<const ActionEncoder*>* ActionEncoder::GetActionEncoders()
{
	static std::vector<const ActionEncoder*> encoders;
	return &encoders;
}

const ActionEncoder* ActionEncoder::FindEncoderByName(const char* name)
{
	auto encoders = GetActionEncoders();

	auto r = std::find_if(encoders->cbegin(), encoders->cend(), [name](const ActionEncoder* encoder)
		{
			return !strcmp(encoder->ShortName(), name) || !strcmp(encoder->PublicName(), name);
		}
	);

	if (r == encoders->end())
		return nullptr;

	return *r;
}

ActionConstructor::ActionConstructor(int actionSize)
{
	m_convention = CallConv_ThisCall;
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
	for (int i = 0; i < numParams; i++)
	{
		ctx->LocalToPhysAddr(params[i + 2], &addr);
		buffer[i] = (cell_t)addr;
	}

	return true;
}

nb_action_ptr ActionConstructor::Execute(IPluginContext* ctx, const cell_t* params, cell_t numParams)
{
	if (m_address == nullptr)
		throw std::runtime_error("Invalid function address");

	if (m_call == nullptr)
		throw std::runtime_error("Call hasn't been finished yet");

	if (m_call->GetParamCount() != numParams)
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

		const ActionEncoder& encoder = *(m_paramEncoder[i]);
		if (!encoder.encode(param, error, maxlength))
		{
			throw std::runtime_error(error);
		}

		vstk.put(param, size);
	}

	m_call->Execute(vstk.get(), nullptr);
	return action;
}

bool ActionConstructor::AddParameter(PassType type, int flags, const ActionEncoder* encoder)
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
		// static ActionEncoder defaultEncoder("g_defaultEncoder", "default");
		// encoder = &defaultEncoder;
	}

	if (info.flags == 0)
		info.flags = PASSFLAG_BYVAL;

	const int maxlength = 255;
	char error[maxlength] = {};
	if (encoder && !encoder->typeinfo(info, error, maxlength))
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

	m_call = bintools->CreateCall(m_address, m_convention, nullptr, m_param, m_paramCount);
	return m_call != nullptr;
}

bool ActionConstructor::SignatureFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor)
{
	if (!config->GetMemSig(constructor, &m_address) || m_address == nullptr)
	{
		ctx->ReportError("Failed to set signature nor address (%s)", constructor);
		return false;
	}

	return true;
}

bool ActionConstructor::AddressFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor)
{
	if (!config->GetAddress(constructor, &m_address) || m_address == nullptr)
	{
		ctx->ReportError("Failed to set signature nor address (%s)", constructor);
		return false;
	}

	return true;
}

bool ActionConstructor::SetupFromConf(IPluginContext* ctx, IGameConfig* config, const ac_data* data)
{
	if (strlen(data->signature) > 0)
	{
		if (!config->GetMemSig(data->signature, &m_address) || m_address == nullptr)
		{
			ctx->ReportError("Failed to set signature (%s)", data->signature);
			return false;
		}
	}

	if (strlen(data->address) > 0)
	{
		if (!config->GetAddress(data->address, &m_address) || m_address == nullptr)
		{
			ctx->ReportError("Failed to set address (%s)", data->address);
			return false;
		}
	}

	SetSize(data->size);

	auto& params = data->params;
	for (auto it = params.cbegin(); it != params.cend(); it++)
	{
		const param_t& param = *it;
		const ActionEncoder* encoder = nullptr;

		if (strlen(param.encoder) > 0)
		{
			encoder = ActionEncoder::FindEncoderByName(param.encoder);

			if (encoder == nullptr)
			{
				ctx->ReportError("Failed to find specified encoder (%s)", param.encoder);
				return false;
			}
		}

		try
		{
			AddParameter(param.info.type, param.info.flags, encoder);
		}
		catch (const std::exception& e)
		{
			ctx->ReportError("%s", e.what());
			return false;
		}
	}

	if (!Finish())
	{
		ctx->ReportError("Failed to finish constructor");
		return false;
	}

	return true;
}

#endif // INCLUDE_ACTIONS_CONSTRUCTOR
