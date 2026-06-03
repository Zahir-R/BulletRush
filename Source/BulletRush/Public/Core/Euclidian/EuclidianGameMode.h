#pragma once

#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "Enemies/Common/Drone.h"
#include "EuclidianGameMode.generated.h"

class UEuclidianPhase;
class AEnemyBase;

UCLASS()
class BULLETRUSH_API AEuclidianGameMode : public ABulletRushGameModeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
		
	UFUNCTION()
	void OnDroneDestroyed(AEnemyBase* DeadEnemy);

	void ChangePhase(UEuclidianPhase* NewPhase);
		
	UPROPERTY()
	UEuclidianPhase* CurrentPhase;

	UPROPERTY()
	TArray<ADrone*> Drones;

	void RefreshDroneList();
	UPROPERTY()
	UObject* CurrentObjective;

	void SetObjective(UObject* NewObjective);
	
	UFUNCTION()
	void OnObservedEnemyDeath(AEnemyBase* Enemy);

	UPROPERTY()
	int32 DeadDroneCount = 0;

	UPROPERTY()
	bool bRedTurretsVulnerable = false;

	FTimerHandle RedTurretVulnerabilityTimer;

	void EnableRedTurretVulnerability(float Duration);

	void DisableRedTurretVulnerability();
protected:

	void SpawnRedDrone();
};