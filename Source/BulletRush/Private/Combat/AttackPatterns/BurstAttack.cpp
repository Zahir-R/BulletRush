#include "Combat/AttackPatterns/BurstAttack.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

void UBurstAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (Params.Count <= 0 || !Spawner || !Spawner->GetOwner()) return;

	struct FBurstData { int32 Remaining; FAttackParams Config; };
	TSharedPtr<FBurstData> BurstData = MakeShared<FBurstData>(FBurstData{ Params.Count, Params });

	FTimerDelegate TimerDel;
	TSharedPtr<FTimerHandle> TimerHandlePtr = MakeShared<FTimerHandle>();
	TWeakObjectPtr<UBulletSpawnerComponent> SpawnerPtr(Spawner);

	TimerDel.BindLambda([SpawnerPtr, BurstData, TimerHandlePtr]() mutable
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
				if (TimerHandlePtr.IsValid())
				{
					SpawnerPtr->GetWorld()->GetTimerManager().ClearTimer(*TimerHandlePtr);
				}
			}
		});

	Spawner->GetWorld()->GetTimerManager().SetTimer(*TimerHandlePtr, TimerDel, Params.SpecialParam, true);
}