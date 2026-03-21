#include "actionsdefs.h"
#include "actions_vtableswap.h"
#include "actions_processor_impl.h"

#include <algorithm>

extern class ActionProcessorShared* g_pActionProcessor;

ActionSwapManager g_swap_manager;

int32_t GetFunctionsCount()
{
	static int32_t count = []() -> int32_t
		{
			int32_t num = 0;
			if (!g_sdkActions.GetGameConfig())
			{
				g_pSM->LogError(myself, "Failed to get functions count: invalid game config ptr");
			}
			else
			{
				if (!g_sdkActions.GetGameConfig()->GetOffset("FunctionsCount", &num))
				{
					g_pSM->LogError(myself, "Failed to get functions count: no entry");
				}
			}

			return num;
		}();

	return count;
}

template<typename T>
static object_internals* GetObjectInternals(T* object)
{
	return reinterpret_cast<object_internals*>(object);
}

class ProcessorVtableManager
{
	static constexpr inline size_t vtable00_size = 200;
	static constexpr inline size_t vtable01_size = 10;
	static constexpr size_t initial_capacity = 256;

	struct vtable
	{
		void* functions[vtable00_size] = {};
	};

	struct object
	{
		vtable* vptr00;
		vtable* vptr01;
	};

	struct slot
	{
		object obj;		
		vtable vt00;
		vtable vt01;
	};

	struct block
	{
		explicit block(size_t count)
			: slots(std::make_unique<slot[]>(count)), size(count)
		{
		}

		std::unique_ptr<slot[]> slots;
		size_t size = 0;
	};

	int32_t vtable_contextual_query_size = 0;
	int32_t vtable_processor_size = 0;

public:
	ProcessorVtableManager()
	{
		auto get_count = [](const char* name)
			{
				int32_t num = 0;
				if (!g_sdkActions.GetGameConfig())
				{
					throw std::runtime_error("Failed to get functions count: invalid game config ptr");
				}
				else
				{
					if (!g_sdkActions.GetGameConfig()->GetOffset(name, &num))
					{
						throw std::runtime_error(std::string("Failed to get functions count: no entry") + name);
					}
				}

				return num;
			};

		vtable_contextual_query_size = get_count("IContextualQuery::Size");
		vtable_processor_size = get_count("ActionProcessor::Size");

		Grow(initial_capacity);
	}

	object_internals* Acquire(nb_action_ptr ptr)
	{
		if (_free_slots.empty())
		{
			const size_t new_capacity = _capacity == 0 ? initial_capacity : _capacity * 2;
			Grow(new_capacity);
		}

		object_internals* result = _free_slots.back();
		_free_slots.pop_back();

		InitializeSlot(reinterpret_cast<slot*>(result), ptr);
		return result;
	}

	void Release(object_internals* internals)
	{
		if (!internals)
			return;

		//InitializeSlot(reinterpret_cast<slot*>(internals));
		_free_slots.push_back(internals);
	}

private:
	void Grow(size_t new_total_capacity)
	{
		if (new_total_capacity <= _capacity)
			return;

		const size_t add_count = new_total_capacity - _capacity;

		auto blk = std::make_unique<block>(add_count);

		for (size_t i = 0; i < add_count; ++i)
		{
			slot& s = blk->slots[i];
			_free_slots.push_back(reinterpret_cast<object_internals*>(&s));
		}

		_blocks.push_back(std::move(blk));
		_capacity = new_total_capacity;
	}

	void InitializeSlot(slot* s, nb_action_ptr action)
	{
		s->obj.vptr00 = &s->vt00; 
		s->obj.vptr01 = &s->vt01;

		object_internals* action_obj = GetObjectInternals(action);
		object_internals* processor_obj = GetObjectInternals(g_pActionProcessor);

		std::memcpy(
			s->vt00.functions,
			action_obj->vt.vptr00->functions,
			sizeof(s->vt00.functions)
		);

		std::memcpy(
			s->vt01.functions,
			action_obj->vt.vptr01->functions,
			sizeof(s->vt01.functions)
		);

		std::memcpy(
			s->vt00.functions,
			processor_obj->vt.vptr00->functions,
			sizeof(void*) * vtable_processor_size
		);

		std::memcpy(
			s->vt01.functions,
			processor_obj->vt.vptr01->functions,
			sizeof(void*) * vtable_contextual_query_size
		);
	}

private:
	std::vector<std::unique_ptr<block>> _blocks;
	std::vector<object_internals*> _free_slots; 
	size_t _capacity = 0;
};

std::unique_ptr<ProcessorVtableManager> g_processor_vtable_manager;

void ActionSwapManager::Init()
{
	g_processor_vtable_manager = std::make_unique<ProcessorVtableManager>();
}

void ActionSwapManager::SwapAction(nb_action_ptr action, const HashFunction* hf)
{
	SwitchFunction(action, hf, true);
}

void ActionSwapManager::UnSwapAction(nb_action_ptr action, const HashFunction* hf)
{
	SwitchFunction(action, hf, false);
}

void ActionSwapManager::SetActionProcessorVtable(nb_action_ptr action)
{
}

void ActionSwapManager::ResetActionVtable(nb_action_ptr action)
{
	auto it = _original_vtable.find(action);	
	
	if (it == _original_vtable.end())
	{
		g_pSM->LogError(myself, "Action has no original vtable!?");
		return;
	}

	object_internals* internals = GetObjectInternals(action);
	
	internals->raw.vptr00 = it->second.raw.vptr00;
	internals->raw.vptr01 = it->second.raw.vptr01;
}

void ActionSwapManager::SwitchFunction(nb_action_ptr action, const HashFunction* hf, bool toOriginal)
{
	auto it = _original_vtable.find(action);

	if (it == _original_vtable.end())
	{
		g_pSM->LogError(myself, "Action has no original vtable!?");
		return;
	}

	object_internals* action_vtable = GetObjectInternals(action);
	object_internals* original_processor_vtable = GetObjectInternals(g_pActionProcessor);
	object_internals* original_action_vtable = &it->second;
	
	vtable* action_vt = !hf->is_contextual ? action_vtable->vt.vptr00 : action_vtable->vt.vptr01;

	if (toOriginal)
	{
		auto vtable_context = !hf->is_contextual ? original_action_vtable->vt.vptr00 : original_action_vtable->vt.vptr01;

		action_vt->functions[hf->offset] = vtable_context->functions[hf->offset];
	}
	else
	{
		auto vtable_context = !hf->is_contextual ? original_processor_vtable->vt.vptr00 : original_processor_vtable->vt.vptr01;

		action_vt->functions[hf->offset] = vtable_context->functions[hf->offset];
	}
}

void ActionSwapManager::BeginActionProcessing(nb_action_ptr action)
{
	if (_original_vtable.find(action) != _original_vtable.end())
		return;

	_original_vtable.try_emplace(action, *GetObjectInternals(action));

	object_internals* slot = g_processor_vtable_manager->Acquire(action);
	_action_processor_slot[action] = slot;

	GetObjectInternals(action)->raw.vptr00 = slot->raw.vptr00;
	GetObjectInternals(action)->raw.vptr01 = slot->raw.vptr01;
}

void ActionSwapManager::StopActionProcessing(nb_action_ptr action)
{
	ResetActionVtable(action);
	_original_vtable.erase(action);

	auto slot_it = _action_processor_slot.find(action);
	if (slot_it != _action_processor_slot.end())
	{
		g_processor_vtable_manager->Release(slot_it->second);
		_action_processor_slot.erase(slot_it);
	}
}

void ActionSwapManager::StopActionProcessing()
{
	// copy
	auto map = _original_vtable;

	for (const auto& [k, v] : map)
	{
		StopActionProcessing(k);
	}
}