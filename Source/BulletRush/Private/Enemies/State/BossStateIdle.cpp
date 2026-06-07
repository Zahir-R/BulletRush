#include "Enemies/State/BossStateIdle.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtr.h"

UBossStateIdle::UBossStateIdle()
{
}

void UBossStateIdle::EnterState(ABossBase* Boss)
{
	if (!Boss->HasActiveWeakPoints())
	{
		TWeakObjectPtr<ABossBase> WeakBoss(Boss);
		Boss->GetWorld()->GetTimerManager().SetTimer(Boss->IntroTimer, [WeakBoss]()
		{
			if (ABossBase* StrongBoss = WeakBoss.Get())
			{
				StrongBoss->ChangeState(StrongBoss->AttackingState);
			}
		}, 0.1f, false);
	}
}

void UBossStateIdle::UpdateState(ABossBase* Boss, float DeltaTime)
{
}

void UBossStateIdle::ExitState(ABossBase* Boss)
{
}

FName UBossStateIdle::GetStateTagName() const
{
	return "Idle";
}
