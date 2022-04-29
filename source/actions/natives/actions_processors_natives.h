#pragma once

template<const char ... s, bool post = false>
cell_t NAT_ActionHandler(IPluginContext* pContext, const cell_t* params)
{
	std::map<std::string, size_t>& offsets = GetOffsetsInfo();
	Action<void>* action = (Action<void>*)params[1];
	std::string name;

	IPluginFunction* listener = NULL;
	ActionsPropagate* propagate = NULL;

	if (!g_pActionsManager->IsValid(action))
	{
		pContext->ReportError("Invalid action passed %X", action);
		return 0;
	}

	listener = pContext->GetFunctionById(params[2]);
	((name += s), ...);

	if constexpr (!post)	propagate = g_pActionsPropagatePre;
	else					propagate = g_pActionsPropagatePost;

	const size_t vtableidx = offsets[name];

	if (vtableidx == 0)
	{
		pContext->ReportFatalError("Failed to find function vtableidx \"%s\"", name.c_str());
		return 0;
	}

	if (listener == NULL)
	{
		if (!propagate->RemoveListener(vtableidx, action, pContext))
		{
			pContext->ReportError("You don't have any listener for %s", name.c_str());
			return 0;
		}
	}
	else
	{
		if (!propagate->AddListener(vtableidx, action, listener))
		{
			pContext->ReportError("You already have listener for %s", name.c_str());
			return 0;
		}
	}

	return 1;
}

cell_t NAT_ActionResultGetReason(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<void>* actionResult = (ActionResult<void>*)params[1];

	if (actionResult == NULL || actionResult->m_reason == NULL)
		return -1;
	
	return pContext->StringToLocal(params[2], params[3], actionResult->m_reason);
}

cell_t NAT_ActionResultSetReason(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<void>* actionResult = (ActionResult<void>*)params[1];

	if (actionResult == NULL)
		return 0;

	char* reason;
	pContext->LocalToString(params[2], &reason);

	actionResult->m_reason = reason;
	return 1;
}

cell_t NAT_ActionResultType(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<void>* actionResult = (ActionResult<void>*)params[1];

	if (actionResult == NULL)
		return 0;

	ActionResultType result = actionResult->m_type;

	if (params[0] > 1)
		actionResult->m_type = (ActionResultType)params[2];

	return result;
}

cell_t NAT_ActionActionType(IPluginContext* pContext, const cell_t* params)
{
	ActionResult<void>* actionResult = (ActionResult<void>*)params[1];

	if (actionResult == NULL)
		return 0;

	Action<void>* action = actionResult->m_action;

	if (params[0] > 1)
	{
		actionResult->m_action = (Action<void>*)params[2];
	}

	return (cell_t)action;
}

cell_t NAT_ActionPriorityType(IPluginContext* pContext, const cell_t* params)
{
	EventDesiredResult<void>* actionResult = (EventDesiredResult<void>*)params[1];

	if (actionResult == NULL)
		return 0;

	EventResultPriorityType result = actionResult->m_priority;

	if (params[0] > 1)
	{
		actionResult->m_priority = (EventResultPriorityType)params[2];
	}

	return (cell_t)result;
}

sp_nativeinfo_t g_ActionProcessorNatives[] =
{
	{ "BehaviorAction.OnStart.set", 								NAT_ActionHandler<'O', 'n', 'S', 't', 'a', 'r', 't'> },
	{ "BehaviorAction.OnUpdate.set", 								NAT_ActionHandler<'O', 'n', 'U', 'p', 'd', 'a', 't', 'e'> },
	{ "BehaviorAction.OnEnd.set", 									NAT_ActionHandler<'O', 'n', 'E', 'n', 'd'> },
	{ "BehaviorAction.OnSuspend.set", 								NAT_ActionHandler<'O', 'n', 'S', 'u', 's', 'p', 'e', 'n', 'd'> },
	{ "BehaviorAction.OnResume.set", 								NAT_ActionHandler<'O', 'n', 'R', 'e', 's', 'u', 'm', 'e'> },
	{ "BehaviorAction.OnInitialContainedAction.set", 				NAT_ActionHandler<'O', 'n', 'I', 'n', 'i', 't', 'i', 'a', 'l', 'C', 'o', 'n', 't', 'a', 'i', 'n', 'e', 'd', 'A', 'c', 't', 'i', 'o', 'n'> },
	{ "BehaviorAction.OnLeaveGround.set", 							NAT_ActionHandler<'O', 'n', 'L', 'e', 'a', 'v', 'e', 'G', 'r', 'o', 'u', 'n', 'd'> },
	{ "BehaviorAction.OnLandOnGround.set", 							NAT_ActionHandler<'O', 'n', 'L', 'a', 'n', 'd', 'O', 'n', 'G', 'r', 'o', 'u', 'n', 'd'> },
	{ "BehaviorAction.OnContact.set", 								NAT_ActionHandler<'O', 'n', 'C', 'o', 'n', 't', 'a', 'c', 't'> },
	{ "BehaviorAction.OnMoveToSuccess.set", 						NAT_ActionHandler<'O', 'n', 'M', 'o', 'v', 'e', 'T', 'o', 'S', 'u', 'c', 'c', 'e', 's', 's'> },
	{ "BehaviorAction.OnMoveToFailure.set", 						NAT_ActionHandler<'O', 'n', 'M', 'o', 'v', 'e', 'T', 'o', 'F', 'a', 'i', 'l', 'u', 'r', 'e'> },
	{ "BehaviorAction.OnStuck.set", 								NAT_ActionHandler<'O', 'n', 'S', 't', 'u', 'c', 'k'> },
	{ "BehaviorAction.OnUnStuck.set", 								NAT_ActionHandler<'O', 'n', 'U', 'n', 'S', 't', 'u', 'c', 'k'> },
	{ "BehaviorAction.OnPostureChanged.set", 						NAT_ActionHandler<'O', 'n', 'P', 'o', 's', 't', 'u', 'r', 'e', 'C', 'h', 'a', 'n', 'g', 'e', 'd'> },
	{ "BehaviorAction.OnAnimationActivityComplete.set", 			NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'A', 'c', 't', 'i', 'v', 'i', 't', 'y', 'C', 'o', 'm', 'p', 'l', 'e', 't', 'e'> },
	{ "BehaviorAction.OnAnimationActivityInterrupted.set",			NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'A', 'c', 't', 'i', 'v', 'i', 't', 'y', 'I', 'n', 't', 'e', 'r', 'r', 'u', 'p', 't', 'e', 'd'> },
	{ "BehaviorAction.OnAnimationEvent.set", 						NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'E', 'v', 'e', 'n', 't'> },
	{ "BehaviorAction.OnIgnite.set", 								NAT_ActionHandler<'O', 'n', 'I', 'g', 'n', 'i', 't', 'e'> },
	{ "BehaviorAction.OnInjured.set", 								NAT_ActionHandler<'O', 'n', 'I', 'n', 'j', 'u', 'r', 'e', 'd'> },
	{ "BehaviorAction.OnKilled.set", 								NAT_ActionHandler<'O', 'n', 'K', 'i', 'l', 'l', 'e', 'd'> },
	{ "BehaviorAction.OnOtherKilled.set", 							NAT_ActionHandler<'O', 'n', 'O', 't', 'h', 'e', 'r', 'K', 'i', 'l', 'l', 'e', 'd'> },
	{ "BehaviorAction.OnSight.set", 								NAT_ActionHandler<'O', 'n', 'S', 'i', 'g', 'h', 't'> },
	{ "BehaviorAction.OnLostSight.set", 							NAT_ActionHandler<'O', 'n', 'L', 'o', 's', 't', 'S', 'i', 'g', 'h', 't'> },
	{ "BehaviorAction.OnThreatChanged.set", 						NAT_ActionHandler<'O', 'n', 'T', 'h', 'r', 'e', 'a', 't', 'C', 'h', 'a', 'n', 'g', 'e', 'd'> },
	{ "BehaviorAction.OnSound.set", 								NAT_ActionHandler<'O', 'n', 'S', 'o', 'u', 'n', 'd'> },
	{ "BehaviorAction.OnSpokeConcept.set", 							NAT_ActionHandler<'O', 'n', 'S', 'p', 'o', 'k', 'e', 'C', 'o', 'n', 'c', 'e', 'p', 't'> },
	{ "BehaviorAction.OnNavAreaChanged.set", 						NAT_ActionHandler<'O', 'n', 'N', 'a', 'v', 'A', 'r', 'e', 'a', 'C', 'h', 'a', 'n', 'g', 'e', 'd'> },
	{ "BehaviorAction.OnModelChanged.set", 							NAT_ActionHandler<'O', 'n', 'M', 'o', 'd', 'e', 'l', 'C', 'h', 'a', 'n', 'g', 'e', 'd'> },
	{ "BehaviorAction.OnPickUp.set", 								NAT_ActionHandler<'O', 'n', 'P', 'i', 'c', 'k', 'U', 'p'> },
	{ "BehaviorAction.OnDrop.set", 									NAT_ActionHandler<'O', 'n', 'D', 'r', 'o', 'p'> },
	{ "BehaviorAction.OnShoved.set", 								NAT_ActionHandler<'O', 'n', 'S', 'h', 'o', 'v', 'e', 'd'> },
	{ "BehaviorAction.OnBlinded.set", 								NAT_ActionHandler<'O', 'n', 'B', 'l', 'i', 'n', 'd', 'e', 'd'> },
	{ "BehaviorAction.OnEnteredSpit.set", 							NAT_ActionHandler<'O', 'n', 'E', 'n', 't', 'e', 'r', 'e', 'd', 'S', 'p', 'i', 't'> },
	{ "BehaviorAction.OnHitByVomitJar.set", 						NAT_ActionHandler<'O', 'n', 'H', 'i', 't', 'B', 'y', 'V', 'o', 'm', 'i', 't', 'J', 'a', 'r'> },
	{ "BehaviorAction.OnCommandAttack.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 't', 't', 'a', 'c', 'k'> },
	{ "BehaviorAction.OnCommandAssault.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 's', 's', 'a', 'u', 'l', 't'> },
	{ "BehaviorAction.OnCommandApproachV.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 'p', 'p', 'r', 'o', 'a', 'c', 'h', 'V'> },
	{ "BehaviorAction.OnCommandApproachE.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 'p', 'p', 'r', 'o', 'a', 'c', 'h', 'E'> },
	{ "BehaviorAction.OnCommandRetreat.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'R', 'e', 't', 'r', 'e', 'a', 't'> },
	{ "BehaviorAction.OnCommandPause.set", 							NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'P', 'a', 'u', 's', 'e'> },
	{ "BehaviorAction.OnCommandResume.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'R', 'e', 's', 'u', 'm', 'e'> },
	{ "BehaviorAction.OnCommandString.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'S', 't', 'r', 'i', 'n', 'g'> },
	{ "BehaviorAction.IsAbleToBlockMovementOf.set", 				NAT_ActionHandler<'I', 's', 'A', 'b', 'l', 'e', 'T', 'o', 'B', 'l', 'o', 'c', 'k', 'M', 'o', 'v', 'e', 'm', 'e', 'n', 't', 'O', 'f'> },

	{ "BehaviorAction.OnStartPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 't', 'a', 'r', 't', true> },
	{ "BehaviorAction.OnUpdatePost.set", 							NAT_ActionHandler<'O', 'n', 'U', 'p', 'd', 'a', 't', 'e', true> },
	{ "BehaviorAction.OnEndPost.set", 								NAT_ActionHandler<'O', 'n', 'E', 'n', 'd', true> },
	{ "BehaviorAction.OnSuspendPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 'u', 's', 'p', 'e', 'n', 'd', true> },
	{ "BehaviorAction.OnResumePost.set", 							NAT_ActionHandler<'O', 'n', 'R', 'e', 's', 'u', 'm', 'e', true> },
	{ "BehaviorAction.OnInitialContainedActionPost.set", 			NAT_ActionHandler<'O', 'n', 'I', 'n', 'i', 't', 'i', 'a', 'l', 'C', 'o', 'n', 't', 'a', 'i', 'n', 'e', 'd', 'A', 'c', 't', 'i', 'o', 'n', true> },
	{ "BehaviorAction.OnLeaveGroundPost.set", 						NAT_ActionHandler<'O', 'n', 'L', 'e', 'a', 'v', 'e', 'G', 'r', 'o', 'u', 'n', 'd', true> },
	{ "BehaviorAction.OnLandOnGroundPost.set", 						NAT_ActionHandler<'O', 'n', 'L', 'a', 'n', 'd', 'O', 'n', 'G', 'r', 'o', 'u', 'n', 'd', true> },
	{ "BehaviorAction.OnContactPost.set", 							NAT_ActionHandler<'O', 'n', 'C', 'o', 'n', 't', 'a', 'c', 't', true> },
	{ "BehaviorAction.OnMoveToSuccessPost.set", 					NAT_ActionHandler<'O', 'n', 'M', 'o', 'v', 'e', 'T', 'o', 'S', 'u', 'c', 'c', 'e', 's', 's', true> },
	{ "BehaviorAction.OnMoveToFailurePost.set", 					NAT_ActionHandler<'O', 'n', 'M', 'o', 'v', 'e', 'T', 'o', 'F', 'a', 'i', 'l', 'u', 'r', 'e', true> },
	{ "BehaviorAction.OnStuckPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 't', 'u', 'c', 'k', true> },
	{ "BehaviorAction.OnUnStuckPost.set", 							NAT_ActionHandler<'O', 'n', 'U', 'n', 'S', 't', 'u', 'c', 'k', true> },
	{ "BehaviorAction.OnPostureChangedPost.set", 					NAT_ActionHandler<'O', 'n', 'P', 'o', 's', 't', 'u', 'r', 'e', 'C', 'h', 'a', 'n', 'g', 'e', 'd', true> },
	{ "BehaviorAction.OnAnimationActivityCompletePost.set", 		NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'A', 'c', 't', 'i', 'v', 'i', 't', 'y', 'C', 'o', 'm', 'p', 'l', 'e', 't', 'e', true> },
	{ "BehaviorAction.OnAnimationActivityInterruptedPost.set", 		NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'A', 'c', 't', 'i', 'v', 'i', 't', 'y', 'I', 'n', 't', 'e', 'r', 'r', 'u', 'p', 't', 'e', 'd', true> },
	{ "BehaviorAction.OnAnimationEventPost.set", 					NAT_ActionHandler<'O', 'n', 'A', 'n', 'i', 'm', 'a', 't', 'i', 'o', 'n', 'E', 'v', 'e', 'n', 't', true> },
	{ "BehaviorAction.OnIgnitePost.set", 							NAT_ActionHandler<'O', 'n', 'I', 'g', 'n', 'i', 't', 'e', true> },
	{ "BehaviorAction.OnInjuredPost.set", 							NAT_ActionHandler<'O', 'n', 'I', 'n', 'j', 'u', 'r', 'e', 'd', true> },
	{ "BehaviorAction.OnKilledPost.set", 							NAT_ActionHandler<'O', 'n', 'K', 'i', 'l', 'l', 'e', 'd', true> },
	{ "BehaviorAction.OnOtherKilledPost.set", 						NAT_ActionHandler<'O', 'n', 'O', 't', 'h', 'e', 'r', 'K', 'i', 'l', 'l', 'e', 'd', true> },
	{ "BehaviorAction.OnSightPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 'i', 'g', 'h', 't', true> },
	{ "BehaviorAction.OnLostSightPost.set", 						NAT_ActionHandler<'O', 'n', 'L', 'o', 's', 't', 'S', 'i', 'g', 'h', 't', true> },
	{ "BehaviorAction.OnThreatChangedPost.set", 					NAT_ActionHandler<'O', 'n', 'T', 'h', 'r', 'e', 'a', 't', 'C', 'h', 'a', 'n', 'g', 'e', 'd', true> },
	{ "BehaviorAction.OnSoundPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 'o', 'u', 'n', 'd', true> },
	{ "BehaviorAction.OnSpokeConceptPost.set", 						NAT_ActionHandler<'O', 'n', 'S', 'p', 'o', 'k', 'e', 'C', 'o', 'n', 'c', 'e', 'p', 't', true> },
	{ "BehaviorAction.OnNavAreaChangedPost.set", 					NAT_ActionHandler<'O', 'n', 'N', 'a', 'v', 'A', 'r', 'e', 'a', 'C', 'h', 'a', 'n', 'g', 'e', 'd', true> },
	{ "BehaviorAction.OnModelChangedPost.set", 						NAT_ActionHandler<'O', 'n', 'M', 'o', 'd', 'e', 'l', 'C', 'h', 'a', 'n', 'g', 'e', 'd', true> },
	{ "BehaviorAction.OnPickUpPost.set", 							NAT_ActionHandler<'O', 'n', 'P', 'i', 'c', 'k', 'U', 'p', true> },
	{ "BehaviorAction.OnDropPost.set", 								NAT_ActionHandler<'O', 'n', 'D', 'r', 'o', 'p', true> },
	{ "BehaviorAction.OnShovedPost.set", 							NAT_ActionHandler<'O', 'n', 'S', 'h', 'o', 'v', 'e', 'd', true> },
	{ "BehaviorAction.OnBlindedPost.set", 							NAT_ActionHandler<'O', 'n', 'B', 'l', 'i', 'n', 'd', 'e', 'd', true> },
	{ "BehaviorAction.OnEnteredSpitPost.set", 						NAT_ActionHandler<'O', 'n', 'E', 'n', 't', 'e', 'r', 'e', 'd', 'S', 'p', 'i', 't', true> },
	{ "BehaviorAction.OnHitByVomitJarPost.set", 					NAT_ActionHandler<'O', 'n', 'H', 'i', 't', 'B', 'y', 'V', 'o', 'm', 'i', 't', 'J', 'a', 'r', true> },
	{ "BehaviorAction.OnCommandAttackPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 't', 't', 'a', 'c', 'k', true> },
	{ "BehaviorAction.OnCommandAssaultPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 's', 's', 'a', 'u', 'l', 't', true> },
	{ "BehaviorAction.OnCommandApproachVPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 'p', 'p', 'r', 'o', 'a', 'c', 'h', 'V', true> },
	{ "BehaviorAction.OnCommandApproachEPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'A', 'p', 'p', 'r', 'o', 'a', 'c', 'h', 'E', true> },
	{ "BehaviorAction.OnCommandRetreatPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'R', 'e', 't', 'r', 'e', 'a', 't', true> },
	{ "BehaviorAction.OnCommandPausePost.set", 						NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'P', 'a', 'u', 's', 'e', true> },
	{ "BehaviorAction.OnCommandResumePost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'R', 'e', 's', 'u', 'm', 'e', true> },
	{ "BehaviorAction.OnCommandStringPost.set", 					NAT_ActionHandler<'O', 'n', 'C', 'o', 'm', 'm', 'a', 'n', 'd', 'S', 't', 'r', 'i', 'n', 'g', true> },
	{ "BehaviorAction.IsAbleToBlockMovementOfPost.set", 			NAT_ActionHandler<'I', 's', 'A', 'b', 'l', 'e', 'T', 'o', 'B', 'l', 'o', 'c', 'k', 'M', 'o', 'v', 'e', 'm', 'e', 'n', 't', 'O', 'f', true> },

	{ "ActionResult.GetReason",										NAT_ActionResultGetReason },
	{ "ActionResult.SetReason",										NAT_ActionResultSetReason },

	{ "ActionResult.type.set",										NAT_ActionResultType },
	{ "ActionResult.type.get",										NAT_ActionResultType },

	{ "ActionResult.action.set",									NAT_ActionActionType },
	{ "ActionResult.action.get",									NAT_ActionActionType },

	{ "ActionDesiredResult.priority.set",							NAT_ActionPriorityType },
	{ "ActionDesiredResult.priority.get",							NAT_ActionPriorityType },

	{ NULL, NULL }
};