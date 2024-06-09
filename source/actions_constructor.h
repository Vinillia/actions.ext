#ifdef INCLUDE_ACTIONS_CONSTRUCTOR

#ifndef _INCLUDE_ACTIONS_CONSTRUCTOR_H
#define _INCLUDE_ACTIONS_CONSTRUCTOR_H

#include <iostream>

#include "actionsdefs.h"

#include <IBinTools.h>
#include <functional>

#include <amtl/am-vector.h>
#include <amtl/am-string.h>

#include "actions_constructor_smc.h"

class ActionConstructor;
class ActionEncoder;

class ActionEncoder
{
protected:
	using param_t = std::add_lvalue_reference_t<void*>;

public:
	ActionEncoder(const char* pubName, const char* symName);
	~ActionEncoder() = default;

	static std::vector<const ActionEncoder*>* GetActionEncoders();
	static const ActionEncoder* FindEncoderByName(const char* name);

	inline const char* ShortName() const;
	inline const char* PublicName() const;

public:
	virtual bool encode(param_t param, char* error, size_t maxlength) const;
	virtual bool typeinfo(SourceMod::PassInfo& param, char* error, size_t maxlength) const;

private:
	ActionEncoder(ActionEncoder&&) = delete;
	ActionEncoder(const ActionEncoder&) = delete;
	ActionEncoder& operator=(const ActionEncoder&) = delete;
	ActionEncoder& operator=(ActionEncoder&&) = delete;

private:
	const char* publicName;
	const char* shortName;
};

inline ActionEncoder::ActionEncoder(const char* pubName, const char* simName) : publicName(pubName), shortName(simName)
{
	GetActionEncoders()->push_back(this);
}

inline bool ActionEncoder::encode(param_t param, char* error, size_t maxlength) const
{
	param = *(void**)param;
	return true;
}

inline bool ActionEncoder::typeinfo(SourceMod::PassInfo& param, char* error, size_t maxlength) const
{
	return true;
}

inline const char* ActionEncoder::ShortName() const
{
	return shortName;
}

inline const char* ActionEncoder::PublicName() const
{
	return publicName;
}


template<typename T>
class TypeEncoder : public ActionEncoder
{
public:
	inline TypeEncoder(const char* pubName, const char* simName) : ActionEncoder(pubName, simName)
	{
		m_param_encoder = nullptr;
		m_type_encoder = nullptr;
	}

	class encode_param_t
	{
	public:
		using param_t = std::add_lvalue_reference_t<std::add_pointer_t<T>>;

		encode_param_t(ActionEncoder::param_t param)
			: m_param((encode_param_t::param_t)(param))
		{
		}

		inline T& operator*()
		{
			return get();
		}

		encode_param_t& operator=(const T& v)
		{
			set(v);
			return *this;
		}

		inline T& get() const
		{
			return reinterpret_cast<T&>(*m_param);
		}

		inline void set(const T& v)
		{
			*m_param = v;
		}

		template<typename K>
		inline void store(K& value)
		{
			m_param = (param_t)value;
		}

	private:
		param_t m_param;
	};

protected:
	using encode_param_ref_t = std::add_lvalue_reference_t<encode_param_t>;

private:
	using ParamEncoder = std::function<bool(encode_param_ref_t param, char* error, size_t maxlength)>;
	using PassEncoder = std::function<bool(SourceMod::PassInfo& info, char* error, size_t maxlength)>;

protected:
	virtual bool encode(param_t param, char* error, size_t maxlength) const override;
	virtual bool typeinfo(SourceMod::PassInfo& info, char* error, size_t maxlength) const override;

protected:
	ParamEncoder m_param_encoder;
	PassEncoder m_type_encoder;
};

template<typename T>
bool TypeEncoder<T>::encode(param_t param, char* error, size_t maxlength) const
{
	if (m_param_encoder == nullptr)
	{
		ke::SafeSprintf(error, maxlength, "%s: failed to encode param: paramEncoder == nullptr", PublicName());
		return false;
	}

	encode_param_t encodeParam(param);
	return m_param_encoder(encodeParam, error, maxlength);
}

template<typename T>
bool TypeEncoder<T>::typeinfo(SourceMod::PassInfo& info, char* error, size_t maxlength) const
{
	if (m_type_encoder == nullptr)
	{
		return ActionEncoder::typeinfo(info, error, maxlength);
	}

	return m_type_encoder(info, error, maxlength);
}

class ActionConstructor
{
	using ac_data = ActionConstructor_SMC::ac_data;
	using param_t = ActionConstructor_SMC::ac_data::param_t;

protected:
	struct stack
	{
		stack(int size)
		{
			vstk = new unsigned char[size];
			vptr = vstk;
		}

		~stack()
		{
			delete[] vstk;
		}

		stack(stack&&) = delete;
		stack(const stack&) = delete;
		stack& operator=(const stack&) = delete;
		stack& operator=(stack&&) = delete;

		inline void put(void* param, int size)
		{
			*(void**)vptr = param;
			vptr += size;
		}

		inline void* get()
		{
			return vstk;
		}

		unsigned char* vstk;
		unsigned char* vptr;
	};

public:
	ActionConstructor(int size = 0);
	~ActionConstructor();

	inline void SetConvention(SourceMod::CallConvention convention);
	inline SourceMod::CallConvention GetConvention() const;

	inline void SetAddress(void* address);
	inline void* GetAddress() const;

	inline void SetSize(size_t size);
	inline size_t GetSize() const;

	bool AddressFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor);
	bool SignatureFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor);
	bool SetupFromConf(IPluginContext* ctx, IGameConfig* config, const ac_data* data);

	bool AddParameter(SourceMod::PassType type, int flags = 0, const ActionEncoder* encoder = nullptr);
	bool Finish();

	bool ConstructParamsBuffer(SourcePawn::IPluginContext* ctx, const cell_t* params, cell_t* buffer, cell_t numParams);
	nb_action_ptr Execute(SourcePawn::IPluginContext* ctx, const cell_t* params, cell_t numParams);

private:
	inline nb_action_ptr AllocateAction() const;

private:
	void* m_address;
	size_t	m_actionSize;

	SourceMod::ICallWrapper* m_call;
	SourceMod::CallConvention m_convention;
	SourceMod::PassInfo m_param[SP_MAX_EXEC_PARAMS];
	const ActionEncoder* m_paramEncoder[SP_MAX_EXEC_PARAMS];

	int		m_paramCount;
	int		m_paramSize;
};

inline nb_action_ptr ActionConstructor::AllocateAction() const
{
	return (nb_action_ptr)(::operator new(m_actionSize));
}

inline void ActionConstructor::SetConvention(SourceMod::CallConvention convention)
{
	m_convention = convention;
}

inline SourceMod::CallConvention ActionConstructor::GetConvention() const
{
	return m_convention;
}

inline void ActionConstructor::SetAddress(void* address)
{
	m_address = address;
}

inline void* ActionConstructor::GetAddress() const
{
	return m_address;
}

inline void ActionConstructor::SetSize(size_t size)
{
	m_actionSize = size;
}

inline size_t ActionConstructor::GetSize() const
{
	return m_actionSize;
}


#endif // !_INCLUDE_ACTIONS_CONSTRUCTOR_H

#endif // INCLUDE_ACTIONS_CONSTRUCTOR
