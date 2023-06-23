#ifndef _INCLUDE_ACTIONS_ENCODERS_H
#define _INCLUDE_ACTIONS_ENCODERS_H

#include "extension.h"
#include "actions_constructor.h"
#include "smsdk_ext.h"

#include <amtl/am-string.h>

class CBaseEntityEncoder : public TypeEncoder<cell_t>
{
public:
	CBaseEntityEncoder() : TypeEncoder("g_entityEncoder", "entity")
	{
		paramEncoder = [](encode_param_ref_t param, char* error, size_t maxlength) -> bool
		{
			cell_t index = *param;
			CBaseEntity* entity = nullptr;
			
			entity = gamehelpers->ReferenceToEntity(index);
			if (entity == nullptr)
			{
				ke::SafeSprintf(error, maxlength, "Invalid entity index (%i)", index);
				return false;
			}
			
			param.store(entity);
			return true;
		};
	}
} g_entityEncoder;

class CVectorEncoder : public TypeEncoder<Vector>
{
public:
	CVectorEncoder() : TypeEncoder("g_vectorEncoder", "vector")
	{
		paramEncoder = [](encode_param_ref_t param, char* error, size_t maxlength) -> bool
		{
			return true;
		};

		passEncoder = [this](PassInfo& info, char* error, size_t maxlength) -> bool
		{
			switch (info.type)
			{
			case PassType_Object:
				info.flags |= PASSFLAG_OASSIGNOP | PASSFLAG_OCTOR;
				info.size = sizeof(Vector);
				info.fields = fields;
				info.numFields = 3;
				break;
			case PassType_Basic:
				info.size = sizeof(Vector*);
				break;
			default:
				ke::SafeSprintf(error, maxlength, "Invalid vector pass type");
				return false;
				break;
			}

			return true;
		};
	}

private:
	static inline ObjectField fields[3] = { ObjectField::Float, ObjectField::Float, ObjectField::Float };
} g_vectorEncoder;


#endif // !_INCLUDE_ACTIONS_ENCODERS_H
