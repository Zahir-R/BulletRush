#include "Enemies/State/BossStateUltimate.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Enemies/BossBase.h"
#include "Player/PlayingPlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Bloodseeker/BloodseekerFacade.h"

UBossStateUltimate::UBossStateUltimate()
{
}

void UBossStateUltimate::EnterState(ABossBase* Boss)
{
	ABloodseekerBoss* B = Cast<ABloodseekerBoss>(Boss);
	if (!B || !B->GetWorld()) return;

	B->SetInvulnerable(true);

	B->bIsMaledictionActive = true;
	B->bIsRuptureActive = true;
	if (B->TargetPlayer)
	{
		B->LastPlayerLocation = B->TargetPlayer->GetActorLocation();
	}

	if (B->BulletSpawner)
	{
		B->BulletSpawner->StopCurrentSequence();
	}

	if (B->FacadeRef)
	{
		B->FacadeRef->PauseBossWaves();
	}

	B->SpawnKamikazeWave(3, 0.3f);

	B->GetWorldTimerManager().SetTimer(Wave2Timer, [Boss]()
	{
		ABloodseekerBoss* Self = Cast<ABloodseekerBoss>(Boss);
		if (Self && Self->bIsMaledictionActive)
		{
			Self->SpawnKamikazeWave(5, 0.25f);
		}
	}, 5.0f, false);

	B->GetWorldTimerManager().SetTimer(Wave3Timer, [Boss]()
	{
		ABloodseekerBoss* Self = Cast<ABloodseekerBoss>(Boss);
		if (Self && Self->bIsMaledictionActive)
		{
			Self->SpawnKamikazeWave(8, 0.2f);
		}
	}, 10.0f, false);

	B->GetWorldTimerManager().SetTimer(DurationTimer, [Boss]()
	{
		if (Boss)
		{
			Boss->ChangeState(Boss->AttackingState);
		}
	}, B->RuptureDuration, false);
}

void UBossStateUltimate::UpdateState(ABossBase* Boss, float DeltaTime)
{
	ABloodseekerBoss* B = Cast<ABloodseekerBoss>(Boss);
	if (!B || !B->bIsRuptureActive || !B->TargetPlayer) return;

	FVector CurrentPlayerLocation = B->TargetPlayer->GetActorLocation();
	float DistanceMoved = FVector::Distance(CurrentPlayerLocation, B->LastPlayerLocation);

	if (DistanceMoved > 0.1f)
	{
		float CalculatedDamage = DistanceMoved * B->RuptureDamageMultiplier;
		UGameplayStatics::ApplyDamage(
			B->TargetPlayer,
			CalculatedDamage,
			B->GetController(),
			B,
			UDamageType::StaticClass()
		);
	}

	B->LastPlayerLocation = CurrentPlayerLocation;
}

void UBossStateUltimate::ExitState(ABossBase* Boss)
{
	ABloodseekerBoss* B = Cast<ABloodseekerBoss>(Boss);
	if (!B) return;

	B->bIsMaledictionActive = false;
	B->bIsRuptureActive = false;
	B->SetInvulnerable(false);

	if (B->GetWorld())
	{
		B->GetWorldTimerManager().ClearTimer(Wave2Timer);
		B->GetWorldTimerManager().ClearTimer(Wave3Timer);
		B->GetWorldTimerManager().ClearTimer(DurationTimer);
	}

	if (B->FacadeRef)
	{
		B->FacadeRef->ResumeBossWaves();
	}
}

FName UBossStateUltimate::GetStateTagName() const
{
	return "Ultimate";
}
