#include "Enemies/Common/Drone.h"
#include "Combat/AttackPatterns.h"
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
}

void ADrone::BeginPlay()
{
    Super::BeginPlay();
    MovementStrategy = MakeShareable(new FSinusoidalSeekMovement(200.f, 2.f, 500.f));
    BeginAttackLoop();
}

void ADrone::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (MovementStrategy.IsValid())
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            FVector Target = PlayerPawn->GetActorLocation();
            FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaSeconds, Target);
            SetActorLocation(NewPos);
        }
    }
}

void ADrone::StartAttack()
{
    if (!BulletSpawner)
    {
        return;
    }

    APawn* PlayerPawn =
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!PlayerPawn)
    {
        return;
    }

    FVector Direction =
        (PlayerPawn->GetActorLocation() -
            GetActorLocation()).GetSafeNormal();

    BulletSpawner->InternalSpawn(
        GetActorLocation(),
        Direction,
        ProjectileSpeed,
        Damage
    );
}

void ADrone::ApplySpeedBuff(float Duration, float FireRateMult, float ProjectileSpeedMult)
{
    AttackInterval = AttackInterval / FireRateMult;
    CurrentProjectileSpeedMultiplier *= ProjectileSpeedMult;
    StopAttackLoop();
    BeginAttackLoop();
}

void ADrone::RemoveSpeedBuff()
{
    AttackInterval = 0.5f;
    CurrentProjectileSpeedMultiplier = 1.f;
    StopAttackLoop();
    BeginAttackLoop();
}
