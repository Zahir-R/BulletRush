#include "../../Public/Combat/AttackPatterns.h"
#include "../../Public/Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"

void FCircleAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

    for (int32 i = 0; i < Params.Count; ++i)
	{
		float Angle = PI * 2.0f / Params.Count * i;
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
		Direction = Direction.GetSafeNormal();

		Spawner->InternalSpawn(Params.Origin, Direction, Params.Speed, Params.Damage);
	}
}

void FSpiralAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

    for (int32 i = 0; i < Params.Count; ++i)
	{
		float Angle = (PI * 2.0f / Params.Count) * i + FMath::DegreesToRadians(Params.SpecialParam);
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
		Direction = Direction.GetSafeNormal();

		float BulletSpeed = Params.Speed;
		BulletSpeed = Params.Speed * (1.0f + (i / (float)FMath::Max(1, Params.Count)) * 1.5f);

		Spawner->InternalSpawn(Params.Origin, Direction, BulletSpeed, Params.Damage);
	}
}

void FSphereAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner) return;

	// Esfera de Fibonacci para distribución casi uniforme en la superficie de la esfera
	const float GoldenAngle = PI * (3.0f - FMath::Sqrt(5.0f));

	for (int32 i = 0; i < Params.Count; ++i)
	{
		float t = (Params.Count > 1) ? (float)i / (float)(Params.Count - 1) : 0.5f;
		float y = 1.0f - 2.0f * t;
		float radius = FMath::Sqrt(FMath::Max(0.0f, 1.0f - y * y));
		float theta = GoldenAngle * i;

		float x = FMath::Cos(theta) * radius;
		float z = FMath::Sin(theta) * radius;

		FVector Direction(x, y, z);
		Direction = Direction.GetSafeNormal();

		Spawner->InternalSpawn(Params.Origin, Direction, Params.Speed, Params.Damage);
	}
}

void FBurstAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
    if (Params.Count <= 0 || !Spawner || !Spawner->GetOwner()) return;

    struct FBurstData { int32 Remaining; FAttackParams Config; };
    TSharedPtr<FBurstData> BurstData = MakeShared<FBurstData>(FBurstData{ Params.Count, Params });

    FTimerDelegate TimerDel;
    FTimerHandle TempHandle;

    TWeakObjectPtr<UBulletSpawnerComponent> SpawnerPtr(Spawner);

    TimerDel.BindLambda([SpawnerPtr, BurstData, TempHandle]() mutable
        {
			if (SpawnerPtr.IsValid() && BurstData->Remaining > 0)
			{
				UWorld* World = SpawnerPtr->GetWorld();
				APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);

				FVector FireDirection;

				if (PlayerPawn)
				{
					FireDirection = (PlayerPawn->GetActorLocation() - BurstData->Config.Origin).GetSafeNormal();
				}
				else
				{
					FireDirection = SpawnerPtr->GetOwner()->GetActorForwardVector();
				}

				SpawnerPtr->InternalSpawn(BurstData->Config.Origin, FireDirection, BurstData->Config.Speed, BurstData->Config.Damage);
				BurstData->Remaining--;
			}
			else if (SpawnerPtr.IsValid())
			{
				SpawnerPtr->GetWorld()->GetTimerManager().ClearTimer(TempHandle);
			}
        });

    Spawner->GetWorld()->GetTimerManager().SetTimer(TempHandle, TimerDel, Params.SpecialParam, true);
}