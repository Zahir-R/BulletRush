#include "Enemies/State/BossStatePhaseTransition.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossStatePhaseTransition::UBossStatePhaseTransition()
{
}

void UBossStatePhaseTransition::EnterState(ABossBase* Boss)
{
	Boss->SetInvulnerable(true);
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->AttackLoopTimer);

	if (!Boss->PhaseTransitionTimer.IsValid())
	{
		Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->PhaseTransitionTimer);
	}

	Boss->AttackIdentifier++;

	TWeakObjectPtr<ABossBase> WeakBoss(Boss);
	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->PhaseTransitionTimer, [WeakBoss]()
	{
		if (ABossBase* StrongBoss = WeakBoss.Get())
		{
			StrongBoss->bHasTransitioned = true;
			StrongBoss->SetInvulnerable(false);
			StrongBoss->ChangeState(StrongBoss->AttackingState);
		}
	}, 5.0f, false);
}

void UBossStatePhaseTransition::ExitState(ABossBase* Boss)
{
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->PhaseTransitionTimer);
}

FName UBossStatePhaseTransition::GetStateTagName() const
{
	return "PhaseTransition";
}
