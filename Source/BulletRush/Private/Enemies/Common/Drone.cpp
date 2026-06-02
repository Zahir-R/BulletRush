#include "Enemies/Common/Drone.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "Combat/AttackPatterns/BurstAttack.h"
#include "Combat/MovementStrategy/SinusoidalSeekMovement.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

ADrone::ADrone()
{
    PrimaryActorTick.bCanEverTick = true;
    AttackInterval = 0.5f;
    ProjectileSpeed = 2000.f;
    Damage = 10.f;
    CurrentProjectileSpeedMultiplier = 1.f;
    MovementStrategy = CreateDefaultSubobject<USinusoidalSeekMovement>(TEXT("SinusoidalMovement"));
}

void ADrone::BeginPlay()
{
    Super::BeginPlay();
    USinusoidalSeekMovement* SinMovement = Cast<USinusoidalSeekMovement>(MovementStrategy);
    if (SinMovement) { SinMovement->Amplitude = 200.f; SinMovement->Frequency = 2.f; SinMovement->StopDistance = 500.f; }
    BeginAttackLoop();
}

void ADrone::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (MovementStrategy)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            FVector Target = PlayerPawn->GetActorLocation();
            FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaSeconds, Target);
            NewPos = ApplyEnemySeparation(NewPos);
            SetActorLocation(NewPos);
        }
    }
}

void ADrone::StartAttack()
{
    UBulletSpawnerComponent* Spawner = FindComponentByClass<UBulletSpawnerComponent>();
    if (!Spawner) return;

    FAttackParams Params;
    Params.Origin = GetActorLocation();
    Params.Damage = Damage;
    Params.Speed = ProjectileSpeed * CurrentProjectileSpeedMultiplier;

    Params.Count = 3;
    Params.SpecialParam = FMath::RandRange(0.1f, 0.3f);

    UBurstAttack().Execute(Spawner, Params);
}

void ADrone::ApplySpeedBuff(float Duration, float FireRateMult, float ProjectileSpeedMult)
{
    // Command/Timer pattern: apply buff and schedule restoration after Duration
    OriginalAttackInterval = AttackInterval;
    OriginalProjectileSpeedMultiplier = CurrentProjectileSpeedMultiplier;

    AttackInterval = AttackInterval / FireRateMult;
    CurrentProjectileSpeedMultiplier *= ProjectileSpeedMult;
    StopAttackLoop();
    BeginAttackLoop();

    // Use a weak pointer in the timer lambda to avoid dangling references
    TWeakObjectPtr<ADrone> WeakThis(this);
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

void ADrone::RemoveSpeedBuff()
{
    // Restore original values if they were saved
    AttackInterval = (OriginalAttackInterval > 0.f) ? OriginalAttackInterval : AttackInterval;
    CurrentProjectileSpeedMultiplier = (OriginalProjectileSpeedMultiplier > 0.f) ? OriginalProjectileSpeedMultiplier : CurrentProjectileSpeedMultiplier;
    StopAttackLoop();
    BeginAttackLoop();
    // Clear timer
    if (GetWorld())
    {
        GetWorldTimerManager().ClearTimer(SpeedBuffRestoreTimerHandle);
    }
}
