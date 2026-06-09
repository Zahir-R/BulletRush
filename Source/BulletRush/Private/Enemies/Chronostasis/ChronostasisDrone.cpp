#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AChronostasisDrone::AChronostasisDrone()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Drone/Nebula_Voyager_texture.Nebula_Voyager_texture'"));
    if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
	
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Drone/M_Nebula_Voyager.M_Nebula_Voyager'"));
	if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);

	PrimaryActorTick.bCanEverTick = true;
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

void AChronostasisDrone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn)
    {
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
        TargetRotation.Roll = 0.0f;
        TargetRotation.Yaw += 180.0f;
        TargetRotation.Pitch *= -1.0f;

        FRotator CurrRotation = GetActorRotation();
        FRotator SmoothRotation = UKismetMathLibrary::RInterpTo(CurrRotation, TargetRotation, DeltaTime, RotationSpeed);
        SetActorRotation(SmoothRotation);
    }
}