#ifndef _INCLUDE_ACTIONS_COMPONENT_H
#define _INCLUDE_ACTIONS_COMPONENT_H

#include "NextBotIntentionInterface.h"
#include "NextBotBehavior.h"

#include "actionsdefs.h"

namespace SourcePawn
{
	class IPluginContext;
	class IPluginFunction;
}

class ActionBehavior : protected Behavior<CBaseEntity>
{
	friend class ActionComponent;
public:
	ActionBehavior(nb_action_ptr initialAction, const char* name, INextBot* bot);

	void Update(CBaseEntity* me, float interval);
	void Resume(CBaseEntity* me);
	void SetName(const char* name);
	const char* GetName() const;

public:
	INextBot* m_nextbot;
};

class ActionComponent : private IIntention
{
	friend class SDKActions;
public:										
	ActionComponent(INextBot* me, SourcePawn::IPluginContext* ctx, SourcePawn::IPluginFunction* plfinitial = nullptr, const char* name = nullptr);
	virtual ~ActionComponent();

private:
	virtual void Reset(void) override;
	virtual void Update(void) override;
	virtual void Upkeep(void) override;
	virtual INextBotEventResponder* FirstContainedResponder(void) const override { return (Behavior<CBaseEntity>*)m_behavior; }
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const override { return NULL; }

	inline nb_action_ptr InitialAction();

	void NotifyReset();
	void NotifyUpdate();
	void NotifyUpkeep();

	static void OnPluginUnloaded(SourcePawn::IPluginContext* ctx);

public:
	inline void SetResetCallback(SourcePawn::IPluginFunction* plfnReset) noexcept;
	inline void SetUpkeepCallback(SourcePawn::IPluginFunction* plfnUpkeep) noexcept;
	inline void SetUpdateCallback(SourcePawn::IPluginFunction* plfnUpdate) noexcept;

	void SetName(const char* name);
	const char* GetName() const;
	void UnRegister();

	static void UnRegisterComponents();
	static bool IsValidComponent(ActionComponent* component) noexcept;

private:			
	CBaseEntity* m_entity;
	ActionBehavior* m_behavior;
	ActionBehavior* m_subehavior;
	const char* m_name;

	SourcePawn::IPluginContext* m_ctx;

	SourcePawn::IPluginFunction* m_plfnInitial;
	SourcePawn::IPluginFunction* m_plfnReset;
	SourcePawn::IPluginFunction* m_plfnUpdate;
	SourcePawn::IPluginFunction* m_plfnUpkeep;
};

inline void ActionComponent::SetResetCallback(SourcePawn::IPluginFunction* plfnReset) noexcept
{
	m_plfnReset = plfnReset;
}

inline void ActionComponent::SetUpkeepCallback(SourcePawn::IPluginFunction* plfnUpkeep) noexcept
{
	m_plfnUpkeep = plfnUpkeep;
}

inline void ActionComponent::SetUpdateCallback(SourcePawn::IPluginFunction* plfnUpdate) noexcept
{
	m_plfnUpdate = plfnUpdate;
}


extern ConVar* NextBotDebugHistory;

#endif // !_INCLUDE_ACTIONS_COMPONENT_H