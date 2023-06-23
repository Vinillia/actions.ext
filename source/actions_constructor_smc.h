#ifndef _INCLUDE_ACTIONS_CONSTRUCTOR_SMC_H
#define _INCLUDE_ACTIONS_CONSTRUCTOR_SMC_H

#include <string>
#include <sm_stringhashmap.h>
#include "extension.h"

namespace SourceMod
{
	static bool IsPassInfoEqual(const PassInfo& left, const PassInfo& right)
	{
		return left.fields == right.fields			&&
				left.flags == right.flags			&&
				left.numFields == right.numFields	&&
				left.size == right.size				&&
				left.type == right.type;
	}
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
			param_t()
			{
				info = SourceMod::PassInfo();
				encoder[0] = '\0';
			}

			bool operator==(const param_t& other) const
			{
				return SourceMod::IsPassInfoEqual(info, other.info) && strcmp(encoder, other.encoder) == 0;
			}
			
			SourceMod::PassInfo info;
			char encoder[32];
		};

		ac_data() : size(0)
		{
			convention = SourceMod::CallConv_ThisCall;
			signature[0] = '\0';
			address[0] = '\0';
			param = nullptr;
		}

		bool operator == (const ac_data& other) const
		{
			return size == other.size			&&
				convention == other.convention	&& 
				params == other.params			&& 
				param == other.param;
		}

		bool operator != (const ac_data& other) const
		{
			return !(*this == other);
		}

		size_t	size;
		char signature[255];
		char address[255];
		SourceMod::CallConvention convention;
		std::vector<param_t> params;

	private:
		param_t* param;
	};

public:
	ActionConstructor_SMC();
	const ac_data* GetACData(const char* name);

private:
	virtual SMCResult ReadSMC_NewSection(const SMCStates* states, const char* name) override;
	virtual SMCResult ReadSMC_KeyValue(const SMCStates* states, const char* key, const char* value) override;
	virtual SMCResult ReadSMC_LeavingSection(const SMCStates* states) override;
	virtual void ReadSMC_ParseStart() override;

private:
	ac_data m_data;
	SMC_State m_state;

	char m_section[255];
	int m_ignoreLevel;
	int m_currentLevel;
	int m_platformLevel;
};

#endif // !_INCLUDE_ACTIONS_CONSTRUCTOR_SMC_H
