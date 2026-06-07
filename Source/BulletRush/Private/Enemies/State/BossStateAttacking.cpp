#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/State/BossStatePhaseTransition.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossStateAttacking::UBossStateAttacking()
{
}

void UBossStateAttacking::EnterState(ABossBase* Boss)
{
	if (Boss->bHasTransitioned)
	{
		Boss->AttackIdentifier = 1;
	}
	else
	{
		Boss->AttackIdentifier = 0;
	}

	Boss->Attack();

	TWeakObjectPtr<ABossBase> WeakBoss(Boss);
	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->AttackLoopTimer, [WeakBoss]()
	{
		if (ABossBase* StrongBoss = WeakBoss.Get())
		{
			StrongBoss->Attack();
		}
	}, Boss->AttackInterval, true);
}

void UBossStateAttacking::UpdateState(ABossBase* Boss, float DeltaTime)
{
	if (Boss->HealthComp && Boss->HealthComp->CurrentHealth < 3000.0f && Boss->AttackIdentifier < 1)
	{
		Boss->ChangeState(Boss->PhaseTransitionState);
	}
}

void UBossStateAttacking::ExitState(ABossBase* Boss)
{
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->AttackLoopTimer);
}

FName UBossStateAttacking::GetStateTagName() const
{
	return "Attacking";
}
