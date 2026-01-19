#pragma once

#ifndef _INCLUDE_ACTIONS_PROCESSOR_IMPL
#define _INCLUDE_ACTIONS_PROCESSOR_IMPL

#include "actions_propagation.h"
#include "actions_pubvars.h"
#include "actionsdefs.h"

class ActionProcessorShared;

struct SwapNode
{
	SwapNode* prev = nullptr;
	SwapNode* next = nullptr;
	class Autoswap* swap = nullptr;
};

struct SwapList
{
	SwapNode head;

	SwapList()
	{
		head.prev = &head;
		head.next = &head;
	}

	bool empty() const { return head.next == &head; }

	static void insert_after(SwapNode* pos, SwapNode* n)
	{
		n->next = pos->next;
		n->prev = pos;
		pos->next->prev = n;
		pos->next = n;
	}

	static void erase(SwapNode* n)
	{
		n->prev->next = n->next;
		n->next->prev = n->prev;
		n->prev = nullptr;
		n->next = nullptr;
	}

	void push_front(SwapNode* n) { insert_after(&head, n); }
	void push_back(SwapNode* n) { insert_after(head.prev, n); }

	SwapNode* front() { return empty() ? nullptr : head.next; }
};

class Autoswap
{
public:
	Autoswap() = delete;
	Autoswap(const void* action);
	~Autoswap();

private:
	Autoswap(Autoswap&&) = delete;
	Autoswap(const Autoswap&) = delete;
	Autoswap& operator=(const Autoswap&) = delete;
	Autoswap& operator=(Autoswap&&) = delete;

private:
	void* m_action;
	SwapNode m_node;
};

bool BeginActionProcessing(nb_action_ptr action);
bool StopActionProcessing(nb_action_ptr action);
void StopActionProcessing();

#endif // !_INCLUDE_ACTIONS_PROCESSOR_IMPL