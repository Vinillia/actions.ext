
#include "extension.h"
#include "actions_component.h"
#include "actions_manager.h"

#include <list>
#include <unordered_map>

#include "actions_custom_legacy.h"
#include "NextBotManager.h"
#include "NextBotBehavior.h"
#include "NextBotIntentionInterface.h"
#include "NextBotInterface.h"

#include "hook.h"

extern ConVar* developer;
extern ConVar* NextBotDebugHistory;
extern ConVar ext_actions_debug_memory;

std::unordered_map<INextBot*, std::list<ActionComponent*>> g_nextbotComponents;

ActionBehavior::ActionBehavior(nb_action_ptr initialAction, const char* name, INextBot* nextbot) : Behavior<CBaseEntity>(initialAction, name)
{
	m_nextbot = nextbot;

	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: NEW BEHAVIOR %s ( 0x%X )", gpGlobals->curtime, GetName(), this);
}

ActionBehavior::~ActionBehavior()
{
	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: DELETE BEHAVIOR %s ( 0x%X )", gpGlobals->curtime, GetName(), this);
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
	g_nextbotComponents[me].push_back(this);

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
	m_behavior = CreateBehavior(me, m_name);

	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: NEW COMPONENT %s ( 0x%X )", gpGlobals->curtime, GetName(), this);
}

ActionComponent::~ActionComponent()
{
	if (ext_actions_debug_memory.GetBool())
		Msg("%.3f: DELETE COMPONENT %s ( 0x%X )", gpGlobals->curtime, GetName(), this);

	if (m_nHandle)
	{
		HandleType_t handle = m_nHandle; 
		HandleSecurity sec(m_ctx->GetIdentity(), nullptr);

		m_nHandle = 0;
		m_handleError = handlesys->FreeHandle(handle, &sec);
		if (m_handleError != HandleError_None)
		{
			Msg("Failed to free action component handle (error %i)", m_handleError);
		}
		else
		{
			Msg("Freed handle (%s)", GetName());
		}
	}

	UnRegister();

	if (m_behavior != nullptr)
		delete m_behavior;
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
	m_behavior->Update(m_entity, GetUpdateInterval());
	NotifyUpdate();
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
		g_actionsManager.Add(action);

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

void ActionComponent::DestroyComponents(CBaseEntity* entity)
{
	INextBot* bot = GetEntityNextbotPointer(entity);

	if (bot == nullptr)
	{
		Msg("DestroyComponents: Failed to get entity nextbot ptr");
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