#ifndef _INCLUDE_ACTIONS_CONSTRUCTOR_H
#define _INCLUDE_ACTIONS_CONSTRUCTOR_H


#include "actionsdefs.h"

#include <IBinTools.h>
#include <functional>

#include <amtl/am-vector.h>
#include <sm_stringhashmap.h>
#include <amtl/am-string.h>

class ActionConstructor;

template<typename T> 
class ActionEncoder;

using Encoder = ActionEncoder<void>;

static inline std::vector<const Encoder*> g_Encoders;

template<typename T>
class ActionEncoder
{
public:
	using param_t = std::add_lvalue_reference_t<std::add_pointer_t<T>>;
	using ParamEncoder_t = std::function<bool(param_t param, char* error, size_t maxlength)>;
	using PassEncoder_t = std::function<bool(SourceMod::PassInfo& info, char* error, size_t maxlength)>;

	static const std::vector<const Encoder*>& GetEncoders()
	{
		return g_Encoders;
	}

public:
	ActionEncoder(const char* pubName, const char* simName) : publicName(pubName), shortName(simName)
	{
		g_Encoders.push_back((Encoder*)this);

		paramEncoder = [](param_t param, char* error, size_t maxlength) -> bool
		{
			param = *(T**)param;
			return true; 
		};

		passEncoder = [](SourceMod::PassInfo& pass, char* error, size_t maxlength) -> bool 
		{ 
			return true; 
		};
	}

	inline bool operator()(param_t param, char* error, size_t maxlength) const;
	inline bool operator()(SourceMod::PassInfo& info, char* error, size_t maxlength) const;

	inline const char* ShortName() const;
	inline const char* PublicName() const;

private:
	ActionEncoder(ActionEncoder&&) = delete;
	ActionEncoder(const ActionEncoder&) = delete;
	ActionEncoder& operator=(const ActionEncoder&) = delete;
	ActionEncoder& operator=(ActionEncoder&&) = delete;

protected:
	ParamEncoder_t paramEncoder;
	PassEncoder_t passEncoder;

private:
	const char* publicName;
	const char* shortName;
};

template<typename T>
bool ActionEncoder<T>::operator()(param_t param, char* error, size_t maxlength) const
{
	if (paramEncoder == nullptr)
	{
		ke::SafeSprintf(error, maxlength, "Param encoder has no target (%s)", publicName);
		return false;
	}

	return paramEncoder(param, error, maxlength);
}

template<typename T>
bool ActionEncoder<T>::operator()(SourceMod::PassInfo& info, char* error, size_t maxlength) const
{
	if (passEncoder == nullptr)
	{
		ke::SafeSprintf(error, maxlength, "Pass encoder has no target (%s)", publicName);
		return false;
	}

	return passEncoder(info, error, maxlength);
}

template<typename T>
inline const char* ActionEncoder<T>::ShortName() const
{
	return shortName;
}

template<typename T>
inline const char* ActionEncoder<T>::PublicName() const
{
	return publicName;
}

class ActionConstructor_SMC : public ITextListener_SMC
{
public:
	enum SMC_State
	{
		SMC_Root,
		SMC_Header,
		SMC_Params
	};

	class ac_data
	{
		friend ActionConstructor_SMC;
	public:
		struct param_t
		{
			param_t() = default;

			SourceMod::PassInfo info;
			std::string encoder;
		};

		ac_data() = default;

		size_t	size;
		std::string signature;
		std::string address;
		std::vector<param_t> params;

	private:
		param_t* param;
	};

private:
	static inline StringHashMap<ac_data> m_acmap;

public:
	ActionConstructor_SMC();

	inline const ac_data* GetACData(const char* name);

private:
	virtual SMCResult ReadSMC_NewSection(const SMCStates* states, const char* name) override;
	virtual SMCResult ReadSMC_KeyValue(const SMCStates* states, const char* key, const char* value) override;
	virtual SMCResult ReadSMC_LeavingSection(const SMCStates* states) override;
	virtual void ReadSMC_ParseStart() override;

private:
	ac_data m_data;
	SMC_State m_state;

	std::string m_section;
	int m_ignoreLevel;
	int m_currentLevel;
	int m_platformLevel;
};


inline const ActionConstructor_SMC::ac_data* ActionConstructor_SMC::GetACData(const char* name)
{
	auto r = m_acmap.find(name);

	if (r.found())
	{
		return &r->value;
	}

	return nullptr;
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

	inline void SetAddress(void* address);
	inline void* GetAddress() const;

	inline void SetSize(size_t size);
	inline size_t GetSize() const;

	bool AddressFromConf(IPluginContext* ctx, IGameConfig* config, const char* constructor);
	bool SetupFromConf(IPluginContext* ctx, IGameConfig* config, const ac_data* data);

	bool AddParameter(SourceMod::PassType type, int flags = 0, const Encoder* encoder = nullptr);
	bool Finish();
	
	bool ConstructParamsBuffer(SourcePawn::IPluginContext* ctx, const cell_t* params, cell_t* buffer, cell_t numParams);
	nb_action_ptr Execute(SourcePawn::IPluginContext* ctx, const cell_t* params, cell_t numParams);

private:
	inline nb_action_ptr AllocateAction() const;

private:
	void*	m_address;
	size_t	m_actionSize;

	SourceMod::ICallWrapper* m_call;
	SourceMod::PassInfo m_param[SP_MAX_EXEC_PARAMS];
	const Encoder* m_paramEncoder[SP_MAX_EXEC_PARAMS];

	int		m_paramCount;
	int		m_paramSize;
};			

inline nb_action_ptr ActionConstructor::AllocateAction() const
{
	return (nb_action_ptr)(::operator new(m_actionSize));
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
