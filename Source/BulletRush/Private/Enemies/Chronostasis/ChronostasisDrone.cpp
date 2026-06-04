#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AChronostasisDrone::AChronostasisDrone()
{
}

void AChronostasisDrone::StartAttack()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;
    FVector Dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    if (UBulletSpawnerComponent* Spawner = FindComponentByClass<UBulletSpawnerComponent>())
    {
        float EffectiveSpeed = ProjectileSpeed * CurrentProjectileSpeedMultiplier;
        Spawner->InternalSpawn(GetActorLocation(), Dir, EffectiveSpeed, Damage);
    }
}

void AChronostasisDrone::ApplySpeedBuff(float Duration, float FireRateMult, float ProjectileSpeedMult)
{
    if (!bSpeedBuffBaselineSaved)
    {
        OriginalAttackInterval = AttackInterval;
        OriginalProjectileSpeedMultiplier = CurrentProjectileSpeedMultiplier;
        bSpeedBuffBaselineSaved = true;
    }

    AttackInterval = OriginalAttackInterval / FireRateMult;
    CurrentProjectileSpeedMultiplier = OriginalProjectileSpeedMultiplier * ProjectileSpeedMult;
    StopAttackLoop();
    BeginAttackLoop();

    TWeakObjectPtr<AChronostasisDrone> WeakThis(this);
    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(SpeedBuffRestoreTimerHandle);
        FTimerDelegate Del = FTimerDelegate::CreateLambda([WeakThis]() {
            if (WeakThis.IsValid())
            {
                WeakThis->RemoveSpeedBuff();
            }
        });
        GetWorldTimerManager().SetTimer(SpeedBuffRestoreTimerHandle, Del, Duration, false);
    }
}

void AChronostasisDrone::RemoveSpeedBuff()
{
    AttackInterval = OriginalAttackInterval;
    CurrentProjectileSpeedMultiplier = OriginalProjectileSpeedMultiplier;
    bSpeedBuffBaselineSaved = false;
    StopAttackLoop();
    BeginAttackLoop();
    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(SpeedBuffRestoreTimerHandle);
    }
}
