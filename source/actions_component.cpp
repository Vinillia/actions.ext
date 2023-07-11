
#include "extension.h"
#include "actions_component.h"
#include "actions_manager.h"
#include "actions_tools.h"

#include <list>
#include <unordered_map>

#include "actions_custom_legacy.h"
#include "NextBotBehavior.h"
#include "NextBotIntentionInterface.h"
#include "NextBotInterface.h"

#include "hook.h"

extern ConVar ext_actions_debug_memory;

std::unordered_map<INextBot*, std::list<ActionComponent*>> g_nextbotComponents;

ActionBehavior::ActionBehavior(nb_action_ptr initialAction, const char* name, INextBot* nextbot) : Behavior<CBaseEntity>(initialAction, name)
{
	if (ext_actions_debug_memory.GetBool())
		MsgSM("%.3f: NEW BEHAVIOR %s ( 0x%X )", gpGlobals->curtime, GetName(), this);

	m_nextbot = nextbot;
}

ActionBehavior::~ActionBehavior()
{
	if (ext_actions_debug_memory.GetBool())
		MsgSM("%.3f: DELETE BEHAVIOR %s ( 0x%X )", gpGlobals->curtime, GetName(), this);
}

void ActionBehavior::Update(CBaseEntity* me, float interval)
{
	Behavior<CBaseEntity>::Update(me, interval);
}

void ActionBehavior::Resume(CBaseEntity* me)
{
	Behavior<CBaseEntity>::Resume(me);
}

ActionComponent::ActionComponent(INextBot* me, SourcePawn::IPluginContext* ctx, SourcePawn::IPluginFunction* plfinitial, const char* name) : IIntention(me)
{
	if (name == nullptr)
		name = "ActionComponent";

	if (ext_actions_debug_memory.GetBool())
		MsgSM("%.3f: NEW COMPONENT %s ( 0x%X )", gpGlobals->curtime, name, this);

	g_nextbotComponents[me].push_back(this);

	m_plfnInitial = plfinitial;
	m_plfnReset = nullptr;
	m_plfnUpdate = nullptr;
	m_plfnUpkeep = nullptr;
	m_subehavior = nullptr;

	m_name = name;

	m_handleError = HandleError_None;
	m_nHandle = g_pHandleSys->CreateHandle(g_sdkActions.GetComponentHT(),
		this,
		ctx->GetIdentity(),
		myself->GetIdentity(),
		&m_handleError);

	m_ctx = ctx;
	m_entity = g_pActionsTools->GetEntity(me);
	m_behavior = CreateBehavior(me, m_name);
}

ActionComponent::~ActionComponent()
{
	if (ext_actions_debug_memory.GetBool())
		MsgSM("%.3f: DELETE COMPONENT %s ( 0x%X )", gpGlobals->curtime, GetName(), this);

	if (m_nHandle)
	{
		HandleType_t handle = m_nHandle; 
		HandleSecurity sec(m_ctx->GetIdentity(), nullptr);

		m_nHandle = 0;
		m_handleError = handlesys->FreeHandle(handle, &sec);
		if (m_handleError != HandleError_None)
		{
			MsgSM("Failed to free action component handle (error %i)", m_handleError);
		}
	}

	UnRegister();

	if (m_behavior != nullptr)
	{
		delete m_behavior;
		m_behavior = nullptr;
	}
}

void ActionComponent::Reset(void)
{
	INextBot* bot = GetBot();

	if (m_behavior != nullptr)
		delete m_behavior;

	m_behavior = CreateBehavior(bot, m_name);
	NotifyReset();
}

void ActionComponent::Upkeep(void)
{
	NotifyUpkeep();
}

void ActionComponent::Update(void)
{
	ResultType result = NotifyUpdate();

	if (result == Pl_Handled)
		return;

	if (m_behavior)
		m_behavior->Update(m_entity, GetUpdateInterval());
}

void ActionComponent::UnRegister()
{
	INextBot* bot = GetBot();

	INextBotComponent* component = bot->m_componentList;
	INextBotComponent* prevComponent = nullptr;

	if (component->m_nextComponent == nullptr)
	{
		bot->m_componentList = nullptr;
		return;
	}

	while (component)
	{
		if (component == this)
		{
			if (prevComponent == nullptr)
			{
				bot->m_componentList = component->m_nextComponent;
				break;
			}
			else
			{
				if (component->m_nextComponent)
				{
					prevComponent->m_nextComponent = this->m_nextComponent;
					break;
				}
				else
				{
					prevComponent->m_nextComponent = nullptr;
					break;
				}
			}
		}

		prevComponent = component;
		component = component->m_nextComponent;
	}
}

inline ActionBehavior* ActionComponent::CreateBehavior(INextBot* bot, const char* name)
{
	nb_action_ptr action = CreateAction();
	ActionBehavior* behavior = new ActionBehavior(action, name, bot);

	if (action)
	{
		g_actionsManager.SetActionActor(action, m_entity);
		g_actionsManager.Add(action);
	}

	return behavior;
}

inline nb_action_ptr ActionComponent::CreateAction()
{
	if (HasHandleError())
	{
		delete this;
		return nullptr;
	}

	nb_action_ptr action = nullptr;

	if (m_plfnInitial)
	{
		m_plfnInitial->PushCell(GetHandle());
		m_plfnInitial->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnInitial->Execute((cell_t*)&action);
	}

	if (action == nullptr)
		action = reinterpret_cast<nb_action_ptr>(new ActionCustomLegacy("ActionComponent"));

	return action;
}

void ActionComponent::DestroyComponents(CBaseEntity* entity)
{
	INextBot* bot = g_pActionsTools->MyNextBotPointer(entity);

	if (bot == nullptr)
	{
		// MsgSM("DestroyComponents: Failed to get entity nextbot ptr");
		return;
	}

	auto& components = g_nextbotComponents[bot];
	for (auto it = components.begin(); it != components.end(); it++)
	{
		delete *it;
	}

	components.clear();
}

void ActionComponent::OnHandleDestroy(HandleType_t type)
{
	if (m_nHandle)
	{
		g_nextbotComponents[m_bot].remove(this);
		delete this;
	}
}

bool ActionComponent::GetHandleApproxSize(HandleType_t type, unsigned int* pSize)
{
	*pSize = sizeof(ActionComponent);
	return true;
}

void ActionComponent::NotifyReset()
{
	if (m_plfnReset)
	{
		m_plfnReset->PushCell(GetHandle());
		m_plfnReset->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnReset->Execute(nullptr);
	}
}

ResultType ActionComponent::NotifyUpdate()
{
	ResultType result = Pl_Continue;

	if (m_plfnUpdate)
	{
		m_plfnUpdate->PushCell(GetHandle());
		m_plfnUpdate->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnUpdate->Execute((cell_t*)&result);
	}

	return result;
}

void ActionComponent::NotifyUpkeep()
{
	if (m_plfnUpkeep)
	{
		m_plfnUpkeep->PushCell(GetHandle());
		m_plfnUpkeep->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnUpkeep->Execute(nullptr);
	}
}