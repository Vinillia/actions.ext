#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <memory>

using vtable_ptr = uintptr_t*;

struct ProcessorFunction;

static constexpr inline size_t vtable00_size = 200;
static constexpr inline size_t vtable01_size = 10;

struct vtable
{
	void* functions[vtable00_size] = {};
};

struct object_internals
{
	union
	{
		struct
		{
			vtable_ptr vptr00;
			vtable_ptr vptr01;
		} raw;

		struct
		{
			vtable* vptr00;
			vtable* vptr01;
		} vt;
	};
};


class ActionSwapManager
{
	friend class ActionsManager;

	static constexpr inline size_t vtable00_size = 200;
	static constexpr inline size_t vtable01_size = 10;

private:
	struct object_processor
	{
		vtable* vptr00;
		vtable* vptr01;
	};

public:
	ActionSwapManager() = default;

	void Init();
	void SwapAction(nb_action_ptr action, const ProcessorFunction* hf);
	void UnSwapAction(nb_action_ptr action, const ProcessorFunction* hf);

	void BeginActionProcessing(nb_action_ptr action);
	void StopActionProcessing(nb_action_ptr action);
	void StopActionProcessing();

private:
	void SetActionProcessorVtable(nb_action_ptr action);
	void ResetActionVtable(nb_action_ptr action);

	void SwitchFunction(nb_action_ptr action, const ProcessorFunction* hf, bool toOriginal);


private:
	std::vector<nb_action_ptr> _swapped_actions;
	std::unordered_map<nb_action_ptr, object_internals> _original_vtable;
	std::unordered_map<nb_action_ptr, object_internals*> _action_processor_slot;
};

extern ActionSwapManager g_swap_manager;
