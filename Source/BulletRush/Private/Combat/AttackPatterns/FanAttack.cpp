#include "Combat/AttackPatterns/FanAttack.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UFanAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
    if (!Spawner || !Spawner->GetOwner()) return;

    AActor* Owner = Spawner->GetOwner();
    AActor* Player = UGameplayStatics::GetPlayerPawn(Owner->GetWorld(), 0);
    if (!Player) return;

    FVector Origin = Params.Origin;
    FVector CenterDir = (Player->GetActorLocation() - Origin).GetSafeNormal();

    float H_Arc_Rad = FMath::DegreesToRadians(Params.SpecialParam);
    if (H_Arc_Rad <= 0.0f) H_Arc_Rad = FMath::DegreesToRadians(60.0f);

    float V_Arc_Rad = H_Arc_Rad * 0.6f;

    int32 TotalBullets = Params.Count;

    FVector CommonOrigin = Origin + CenterDir * 150.0f;

    FVector Forward = CenterDir;
    FVector WorldUp = FVector::UpVector;

    FVector Right = FVector::CrossProduct(Forward, WorldUp).GetSafeNormal();
    if (Right.IsNearlyZero()) Right = FVector::RightVector;

    for (int32 i = 0; i < TotalBullets; ++i)
    {
        float H_Angle = FMath::FRandRange(-H_Arc_Rad * 0.5f, H_Arc_Rad * 0.5f);
        float V_Angle = FMath::FRandRange(-V_Arc_Rad * 0.5f, V_Arc_Rad * 0.5f);

        FVector Dir = Forward;
        Dir = Dir.RotateAngleAxis(FMath::RadiansToDegrees(H_Angle), WorldUp);
        FVector LocalRight = FVector::CrossProduct(Dir, WorldUp).GetSafeNormal();
        if (LocalRight.IsNearlyZero()) LocalRight = Right;
        Dir = Dir.RotateAngleAxis(FMath::RadiansToDegrees(V_Angle), LocalRight);
        Dir = Dir.GetSafeNormal();

        Spawner->InternalSpawnAt(CommonOrigin, Dir, Params.Speed, Params.Damage);
    }
}
