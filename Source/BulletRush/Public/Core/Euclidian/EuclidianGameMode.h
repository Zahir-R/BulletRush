#pragma once

#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "EuclidianGameMode.generated.h"

class UEuclidianPhase;
class AEnemyBase;

UCLASS()
class BULLETRUSH_API AEuclidianGameMode : public ABulletRushGameModeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	void ChangePhase(UEuclidianPhase* NewPhase);
		
	UPROPERTY()
	UEuclidianPhase* CurrentPhase;

	UPROPERTY()
	UObject* CurrentObjective;

	void SetObjective(UObject* NewObjective);
	
	UFUNCTION()
	void OnObservedEnemyDeath(AEnemyBase* Enemy);

	UPROPERTY()
	int32 DeadDroneCount = 0;

	void RegisterDroneDeath();
};