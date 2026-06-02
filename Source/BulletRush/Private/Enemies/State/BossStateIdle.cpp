#include "Enemies/State/BossStateIdle.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossStateIdle::UBossStateIdle()
{
}

void UBossStateIdle::EnterState(ABossBase* Boss)
{
	if (!Boss->HasActiveWeakPoints())
	{
		Boss->GetWorld()->GetTimerManager().SetTimer(Boss->IntroTimer, [Boss]()
		{
			if (Boss) Boss->ChangeState(Boss->AttackingState);
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
