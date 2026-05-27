#include "Enemies/BossState.h"
#include "Enemies/BossBase.h"
#include "Components/HealthComponent.h"
#include "Components/WeakPointComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBossState::UBossState()
{
}

void UBossState::EnterState(ABossBase* Boss)
{
}

void UBossState::UpdateState(ABossBase* Boss, float DeltaTime)
{
}

void UBossState::ExitState(ABossBase* Boss)
{
}

FName UBossState::GetStateTagName() const
{
	return NAME_None;
}

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

UBossStateIdle::UBossStateIdle()
{
}

void UBossStateIdle::EnterState(ABossBase* Boss)
{
	if (Boss->TestWeak)
	{
		Boss->TestWeak->OnDestroyedEvent.AddDynamic(Boss, &ABossBase::OnTestWeakDestroyed);
	}
}

void UBossStateIdle::UpdateState(ABossBase* Boss, float DeltaTime)
{
}

void UBossStateIdle::ExitState(ABossBase* Boss)
{
	if (Boss->TestWeak)
	{
		Boss->TestWeak->OnDestroyedEvent.RemoveDynamic(Boss, &ABossBase::OnTestWeakDestroyed);
	}
}

FName UBossStateIdle::GetStateTagName() const
{
	return "Idle";
}

UBossStateAttacking::UBossStateAttacking()
{
}

void UBossStateAttacking::EnterState(ABossBase* Boss)
{
	Boss->HealthComp->SetInvulnerable(false);

	if (Boss->bHasTransitioned)
	{
		Boss->AttackIdentifier = 1;
	}
	else
	{
		Boss->AttackIdentifier = 0;
	}

	Boss->Attack();

	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->AttackLoopTimer, [Boss]()
	{
		if (Boss)
		{
			Boss->Attack();
		}
	}, 3.0f, true);
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

	Boss->GetWorld()->GetTimerManager().SetTimer(Boss->PhaseTransitionTimer, [Boss]()
	{
		if (Boss)
		{
			Boss->bHasTransitioned = true;
			Boss->SetInvulnerable(false);
			Boss->ChangeState(Boss->AttackingState);
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

UBossStateDead::UBossStateDead()
{
}

void UBossStateDead::EnterState(ABossBase* Boss)
{
}

void UBossStateDead::ExitState(ABossBase* Boss)
{
}

FName UBossStateDead::GetStateTagName() const
{
	return "Dead";
}
