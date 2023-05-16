
#include "extension.h"
#include "actions_component.h"
#include "actions_manager.h"

#include <list>

#include "actions_custom_legacy.h"
#include "NextBotManager.h"
#include "NextBotBehavior.h"
#include "NextBotIntentionInterface.h"
#include "NextBotInterface.h"

#include "hook.h"

std::list<ActionComponent*> g_actionComponents;
ConVar* NextBotDebugHistory = nullptr;
extern ConVar ext_actions_debug_memory;

ActionBehavior::ActionBehavior(nb_action_ptr initialAction, const char* name, INextBot* nextbot) : Behavior<CBaseEntity>(initialAction, name)
{
	m_nextbot = nextbot;
}

void ActionBehavior::Update(CBaseEntity* me, float interval)
{
	Behavior<CBaseEntity>::Update(me, interval);

	if (g_sdkActions.IsNextBotDebugSupported() && m_action && TheNextBots().IsDebugging(NEXTBOT_BEHAVIOR))
	{
		CFmtStr msg;
		m_nextbot->DisplayDebugText(msg.sprintf("%s: %s", GetName(), m_action->DebugString()));
	}
}

void ActionBehavior::Resume(CBaseEntity* me)
{
	Behavior<CBaseEntity>::Resume(me);

	if (g_sdkActions.IsNextBotDebugSupported() && m_action && TheNextBots().IsDebugging(NEXTBOT_BEHAVIOR))
	{
		CFmtStr msg;
		m_nextbot->DisplayDebugText(msg.sprintf("%s: %s", GetName(), m_action->DebugString()));
	}
}

void ActionBehavior::SetName(const char* name)
{
	m_name.sprintf("%s", name);
}

const char* ActionBehavior::GetName() const
{
	return m_name;
}

ActionComponent::ActionComponent(INextBot* me, SourcePawn::IPluginContext* ctx, SourcePawn::IPluginFunction* plfinitial, const char* name) : IIntention(me)
{
	// g_actionComponents.push_back(this);
	m_plfnInitial = plfinitial;
	m_plfnReset = nullptr;
	m_plfnUpdate = nullptr;
	m_plfnUpkeep = nullptr;
	m_subehavior = nullptr;
	m_name = name ? name : "ActionComponent";

	m_handleError = HandleError_None;
	m_nHandle = g_pHandleSys->CreateHandle(g_sdkActions.GetComponentHT(),
		this,
		ctx->GetIdentity(),
		myself->GetIdentity(),
		&m_handleError);

	m_ctx = ctx;
	m_entity = (CBaseEntity*)me->GetEntity();
	m_behavior = new ActionBehavior(InitialAction(), m_name, me);

	if (m_behavior->m_action)
		g_actionsManager.Add(m_behavior->m_action);

	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: NEW COMPONENT 0x%X", gpGlobals->curtime, this);
}

ActionComponent::~ActionComponent()
{
	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: DELETE COMPONENT 0x%X", gpGlobals->curtime, this);

	// g_actionComponents.remove(this);
	delete m_behavior;
}

void ActionComponent::Reset(void)
{
	INextBot* bot = m_behavior->m_nextbot;
	delete m_behavior; 
	m_behavior = new ActionBehavior(InitialAction(), m_name, bot);

	NotifyReset();
}

void ActionComponent::Upkeep(void)
{
	NotifyUpkeep();
}

void ActionComponent::Update(void)
{
	m_behavior->Update(m_entity, GetUpdateInterval());
	NotifyUpdate();
}

void ActionComponent::OnPluginUnloaded(IPluginContext* ctx)
{
	for (auto iter = g_actionComponents.begin(); iter != g_actionComponents.end();)
	{
		auto component = *iter;

		if (component == nullptr)
			break;

		if (component->m_ctx == ctx)
		{
			component->UnRegister();
			iter++;
			delete component;
		}
		else
		{
			iter++;
		}
	}
}

void ActionComponent::SetName(const char* name)
{
	if (m_behavior)
		m_behavior->SetName(name);
}

const char* ActionComponent::GetName() const
{
	if (m_behavior)
		return m_behavior->GetName();

	return nullptr;
}

void ActionComponent::UnRegister()
{
	INextBot* bot = GetBot();

	INextBotComponent* component = bot->m_componentList;
	INextBotComponent* prevComponent = nullptr;

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

inline nb_action_ptr ActionComponent::InitialAction()
{
	if (HasHandleError())
	{
		delete this;
		return nullptr;
	}

	nb_action_ptr initialAction = nullptr;

	if (m_plfnInitial)
	{
		m_plfnInitial->PushCell(GetHandle());
		m_plfnInitial->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnInitial->Execute((cell_t*)&initialAction);
	}

	if (initialAction == nullptr)
		initialAction = reinterpret_cast<nb_action_ptr>(new ActionCustomLegacy("ActionComponent"));

	return initialAction;
}

inline void ActionComponent::NotifyReset()
{
	if (m_plfnReset)
	{
		m_plfnReset->PushCell(GetHandle());
		m_plfnReset->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnReset->Execute(nullptr);
	}
}

inline void ActionComponent::NotifyUpdate()
{
	if (m_plfnUpdate)
	{
		m_plfnUpdate->PushCell(GetHandle());
		m_plfnUpdate->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnUpdate->Execute(nullptr);
	}
}

inline void ActionComponent::NotifyUpkeep()
{
	if (m_plfnUpkeep)
	{
		m_plfnUpkeep->PushCell(GetHandle());
		m_plfnUpkeep->PushCell(gamehelpers->EntityToBCompatRef(m_entity));
		m_plfnUpkeep->Execute(nullptr);
	}
}

void ActionComponent::UnRegisterComponents()
{
	for (auto iter = g_actionComponents.begin(); iter != g_actionComponents.end();)
	{
		auto component = *iter;

		if (component == nullptr)
			break;

		component->UnRegister();
		iter++;
		delete component;
	}
}

bool ActionComponent::IsValidComponent(ActionComponent* comp) noexcept
{
	/*
	* Handle system will save us 
	auto r = std::find(g_actionComponents.cbegin(), g_actionComponents.cend(), comp);
	return r != g_actionComponents.cend();
	*/

	return true;
}


void ActionComponent::OnHandleDestroy(HandleType_t type)
{
	UnRegister();
	delete this;
}

bool ActionComponent::GetHandleApproxSize(HandleType_t type, unsigned int* pSize)
{
	*pSize = sizeof(ActionComponent);
	return true;
}


void ActionDebugMsg(Behavior<CBaseEntity>* behavior, Action<CBaseEntity>* action, CBaseEntity* actor, std::string_view method)
{
	if (!g_sdkActions.IsNextBotDebugSupported())
		return;

	INextBot* nextbot = GetEntityNextbotPointer(actor);

	if (nextbot && (nextbot->IsDebugging(NEXTBOT_EVENTS) || (NextBotDebugHistory && NextBotDebugHistory->GetBool())))
	{															
		nextbot->DebugConColorMsg(NEXTBOT_EVENTS, Color(100, 100, 100, 255), "%3.2f: %s:%s: %s received EVENT %s\n", gpGlobals->curtime, nextbot->GetDebugIdentifier(), behavior->GetName(), action->GetFullName(), method.data());
	}
}

void ActionDebugMsg(Behavior<CBaseEntity>* behavior, Action<CBaseEntity>* action, CBaseEntity* actor, std::string_view method, const IActionResult<CBaseEntity>& result)
{
	if (!g_sdkActions.IsNextBotDebugSupported())
		return;

	INextBot* nextbot = GetEntityNextbotPointer(actor);

	if (nextbot && result.IsRequestingChange() && (nextbot->IsDebugging(NEXTBOT_BEHAVIOR) || (NextBotDebugHistory && NextBotDebugHistory->GetBool())))
	{																						
		nextbot->DebugConColorMsg(NEXTBOT_BEHAVIOR, Color(255, 255, 0, 255), "%3.2f: %s:%s: ", gpGlobals->curtime, nextbot->GetDebugIdentifier(), behavior->GetName()); 
		nextbot->DebugConColorMsg(NEXTBOT_BEHAVIOR, Color(255, 255, 255, 255), "%s ", action->GetFullName());			
		nextbot->DebugConColorMsg(NEXTBOT_BEHAVIOR, Color(255, 255, 0, 255), "reponded to EVENT %s with ", method.data());	
		nextbot->DebugConColorMsg(NEXTBOT_BEHAVIOR, Color(255, 0, 0, 255), "%s %s ", result.GetTypeName(), result.m_action ? result.m_action->GetName() : "");
		nextbot->DebugConColorMsg(NEXTBOT_BEHAVIOR, Color(0, 255, 0, 255), "%s\n", result.m_reason ? result.m_reason : "");
	}
}
