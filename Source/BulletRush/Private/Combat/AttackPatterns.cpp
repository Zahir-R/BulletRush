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