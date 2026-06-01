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

	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->StunnedTimer, [Boss]()
	{
		if (Boss)
		{
			Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->AttackLoopTimer);
			Boss->ChangeState(Boss->AttackingState);
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
