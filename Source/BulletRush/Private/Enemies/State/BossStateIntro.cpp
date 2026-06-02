#include "Enemies/State/BossStateIntro.h"
#include "Enemies/State/BossStateIdle.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossStateIntro::UBossStateIntro()
{
}

void UBossStateIntro::EnterState(ABossBase* Boss)
{
	Boss->HealthComp->SetInvulnerable(true);

	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->IntroTimer, [Boss]()
	{
		if (Boss)
		{
			Boss->ChangeState(Boss->IdleState);
		}
	}, 2.0f, false);
}

void UBossStateIntro::ExitState(ABossBase* Boss)
{
	Boss->GetWorld()->GetTimerManager().ClearTimer(Boss->IntroTimer);
}

FName UBossStateIntro::GetStateTagName() const
{
	return "Intro";
}
