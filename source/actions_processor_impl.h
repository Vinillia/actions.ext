#pragma once

#ifndef _INCLUDE_ACTIONS_PROCESSOR_IMPL
#define _INCLUDE_ACTIONS_PROCESSOR_IMPL

#include "actions_propagation.h"
#include "actions_pubvars.h"
#include "actionsdefs.h"

class ActionProcessorShared;

class Autoswap
{
public:
	Autoswap() = delete;
	Autoswap(const ActionProcessorShared* action);
	~Autoswap();

private:
	Autoswap(Autoswap&&) = delete;
	Autoswap(const Autoswap&) = delete;
	Autoswap& operator=(const Autoswap&) = delete;
	Autoswap& operator=(Autoswap&&) = delete;
private:
	ActionProcessorShared* m_action;
};

bool BeginActionProcessing(nb_action_ptr action);
bool StopActionProcessing(nb_action_ptr action);
void StopActionProcessing();

#endif // !_INCLUDE_ACTIONS_PROCESSOR_IMPL