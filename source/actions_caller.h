#ifndef _INCLUDE_ACTIONS_CALLER_H
#define _INCLUDE_ACTIONS_CALLER_H

cell_t NAT_caller_ActionConsctructor(IPluginContext* ctx, const cell_t* params)
{
	ActionConstructor* constuctor = new ActionConstructor(params[1]);
	Handle_t handle;
	HandleError error;

	if ((handle = handlesys->CreateHandle(g_sdkActions.GetConstructorHT(),
		constuctor,
		ctx->GetIdentity(),
		myself->GetIdentity(),
		&error)) == BAD_HANDLE)
	{
		ctx->ReportError("Failed to create constructor handle (error %i)", error);
		return 0;
	}

	return handle;
}

cell_t NAT_caller_AddParameter(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	PassType passType = (PassType)params[2];
	size_t flags = (size_t)params[3];
	Encoder* encoder = (Encoder*)params[4];

	return constuctor->AddParameter(passType, flags, encoder);
}

cell_t NAT_caller_Execute(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	cell_t count = params[0];
	nb_action_ptr action = nullptr;

	try
	{
		action = constuctor->Execute(ctx, params, count);

		if (action != nullptr)
		{
			g_actionsManager.SetActionActor(action, nullptr);
			g_actionsManager.Add(action);
		}
	}	
	catch(const std::exception& e)
	{
		ctx->ReportError("Failed to execute constructor: %s", e.what());
	}

	return (cell_t)action;
}

cell_t NAT_caller_Finish(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	return constuctor->Finish();
}

cell_t NAT_caller_AddressFromConf(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	IGameConfig* config = nullptr;
	HandleError err;
	
	if ((config = gameconfs->ReadHandle(params[2], ctx->GetIdentity(), &err)) == nullptr)
	{
		ctx->ReportError("Invalid gamedata handle %x (error %d)", params[2], err);
		return 0;
	}

	char* key;
	ctx->LocalToString(params[3], &key);
	return constuctor->AddressFromConf(ctx, config, key);
}

cell_t NAT_caller_Address(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	void* addr = (void*)params[2];
	if ((uintptr_t)addr <= 0x1000)
	{
		ctx->ReportError("Invalid function address (0x%X)", addr);
		return 0;
	}

	if (params[0] > 1)
	{
		constuctor->SetAddress((void*)params[2]);
	}

	return (cell_t)constuctor->GetAddress();
}

cell_t NAT_caller_Size(IPluginContext* ctx, const cell_t* params)
{
	HandleSecurity sec(nullptr, myself->GetIdentity());
	Handle_t handle = params[1];
	HandleError error;

	ActionConstructor* constuctor = nullptr;
	if ((error = g_pHandleSys->ReadHandle(handle, g_sdkActions.GetConstructorHT(), &sec, (void**)&constuctor))
		!= HandleError_None)
	{
		ctx->ReportError("Invalid component handle %x (error %d)", handle, error);
		return 0;
	}

	if (params[0] > 1)
	{
		constuctor->SetSize(params[2]);
	}

	return constuctor->GetSize();
}

cell_t NAT_caller_SetupFromConf(IPluginContext* ctx, const cell_t* params)
{
	IGameConfig* config = nullptr;
	HandleError err;

	if ((config = gameconfs->ReadHandle(params[1], ctx->GetIdentity(), &err)) == nullptr)
	{
		ctx->ReportError("Invalid gamedata handle %x (error %d)", params[2], err);
		return 0;
	}

	char* key;
	ctx->LocalToString(params[2], &key);
	
	auto ac_data = g_pActionConstructorSMC->GetACData(key);
	if (ac_data == nullptr)
	{
		ctx->ReportError("Invalid action constructor (%s)", key);
		return 0;
	}

	std::unique_ptr<ActionConstructor> constuctor = std::make_unique<ActionConstructor>();
	if (!constuctor->SetupFromConf(ctx, config, ac_data))
		return false;

	Handle_t handle;
	HandleError error;

	if ((handle = handlesys->CreateHandle(g_sdkActions.GetConstructorHT(),
		constuctor.get(),
		ctx->GetIdentity(),
		myself->GetIdentity(),
		&error)) == BAD_HANDLE)
	{
		ctx->ReportError("Failed to create constructor handle (error %i)", error);
		return 0;
	}

	constuctor.release();
	return handle;
}

sp_nativeinfo_t g_actionsNativesCaller[] =
{
	{ "ActionConstructor.ActionConstructor", NAT_caller_ActionConsctructor },
	{ "ActionConstructor.SetupFromConf", NAT_caller_SetupFromConf },
	{ "ActionConstructor.AddressFromConf", NAT_caller_AddressFromConf },
	{ "ActionConstructor.AddParameter", NAT_caller_AddParameter },
	{ "ActionConstructor.Finish", NAT_caller_Finish },
	{ "ActionConstructor.Execute", NAT_caller_Execute },
	{ "ActionConstructor.Address", NAT_caller_Address },
	{ "ActionConstructor.Size", NAT_caller_Size },

	{ NULL, NULL }
};

#endif // !_INCLUDE_ACTIONS_CALLER_H
