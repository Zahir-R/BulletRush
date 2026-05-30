#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AChronostasisDrone::AChronostasisDrone()
{
    // Inherits values from ADrone, custom values can be set here if needed
}

void AChronostasisDrone::StartAttack()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;
    FVector Dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    // Use BulletSpawner if present
    if (UBulletSpawnerComponent* Spawner = FindComponentByClass<UBulletSpawnerComponent>())
    {
        float EffectiveSpeed = ProjectileSpeed * CurrentProjectileSpeedMultiplier;
        Spawner->InternalSpawn(GetActorLocation(), Dir, EffectiveSpeed, Damage);
    }
}
