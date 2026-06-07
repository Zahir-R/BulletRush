#include "Enemies/State/BossStateStunned.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossStateStunned::UBossStateStunned()
{
}

void UBossStateStunned::EnterState(ABossBase* Boss)
{
	Boss->HealthComp->SetInvulnerable(false);

	TWeakObjectPtr<ABossBase> WeakBoss(Boss);
	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->StunnedTimer, [WeakBoss]()
	{
		if (ABossBase* StrongBoss = WeakBoss.Get())
		{
			StrongBoss->GetWorld()->GetTimerManager().ClearTimer(StrongBoss->AttackLoopTimer);
			StrongBoss->ChangeState(StrongBoss->AttackingState);
		}
	}, 5.0f, false);
}

void UBossStateStunned::ExitState(ABossBase* Boss)
{
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->StunnedTimer);
}

FName UBossStateStunned::GetStateTagName() const
{
	return "Stunned";
}
