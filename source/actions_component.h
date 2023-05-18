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

	virtual ~ActionBehavior() override;
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
	friend class IActionComponentDispatch;

public:										
	ActionComponent(INextBot* me, SourcePawn::IPluginContext* ctx, SourcePawn::IPluginFunction* plfinitial = nullptr, const char* name = nullptr);
	virtual ~ActionComponent() override;

private:
	virtual void Reset(void) override;
	virtual void Update(void) override;
	virtual void Upkeep(void) override;
	virtual INextBotEventResponder* FirstContainedResponder(void) const override { return (Behavior<CBaseEntity>*)m_behavior; }
	virtual INextBotEventResponder* NextContainedResponder(INextBotEventResponder* current) const override { return NULL; }

	nb_action_ptr CreateAction();
	ActionBehavior* CreateBehavior(INextBot* bot, const char* name);

	void NotifyReset();
	void NotifyUpdate();
	void NotifyUpkeep();

	void OnHandleDestroy(HandleType_t type);
	bool GetHandleApproxSize(HandleType_t type, unsigned int* pSize);

public:
	inline void SetResetCallback(SourcePawn::IPluginFunction* plfnReset) noexcept;
	inline void SetUpkeepCallback(SourcePawn::IPluginFunction* plfnUpkeep) noexcept;
	inline void SetUpdateCallback(SourcePawn::IPluginFunction* plfnUpdate) noexcept;

	inline bool HasHandleError() const noexcept;
	inline SourceMod::HandleType_t GetHandle() const noexcept;
	inline SourceMod::HandleError GetHandleError() const noexcept;

	void SetName(const char* name);
	const char* GetName() const;
	void UnRegister();

	static void DestroyComponents(CBaseEntity* entity);
	static bool IsValidComponent(ActionComponent* component) noexcept;

private:			
	CBaseEntity* m_entity;
	ActionBehavior* m_behavior;
	ActionBehavior* m_subehavior;
	const char* m_name;

	SourceMod::HandleError m_handleError;
	SourceMod::HandleType_t m_nHandle;
	SourcePawn::IPluginContext* m_ctx;

	SourcePawn::IPluginFunction* m_plfnInitial;
	SourcePawn::IPluginFunction* m_plfnReset;
	SourcePawn::IPluginFunction* m_plfnUpdate;
	SourcePawn::IPluginFunction* m_plfnUpkeep;
};

inline bool ActionComponent::HasHandleError() const noexcept
{
	return m_handleError != HandleError_None;
}

inline SourceMod::HandleType_t ActionComponent::GetHandle() const noexcept
{
	return m_nHandle;
}

inline SourceMod::HandleError ActionComponent::GetHandleError() const noexcept
{
	return m_handleError;
}

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
