#pragma once

#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "Enemies/Common/Drone.h"
#include "Enemies/Bloodseeker/SkySphereWorld.h"
#include "EuclidianGameMode.generated.h"

class UEuclidianPhase;
class AEnemyBase;
class USoundBase;

UCLASS()
class BULLETRUSH_API AEuclidianGameMode : public ABulletRushGameModeBase
{
	GENERATED_BODY()

public:

	AEuclidianGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnPlayerDeath();

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* CombatSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	USoundBase* AmbientSong;

	UPROPERTY(EditAnywhere, Category = "Music")
	float CombatStartOffset = 0.0f;

	UFUNCTION()
	void OnDroneDestroyed(AEnemyBase* DeadEnemy);

	void ChangePhase(UEuclidianPhase* NewPhase);
		
	UPROPERTY()
	UEuclidianPhase* CurrentPhase;

	UPROPERTY()
	TArray<ADrone*> Drones;

	UPROPERTY()
	ASkySphereWorld* SkySphere;

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

	TArray<AActor*>FoundDrones;

	FTimerHandle RedTurretVulnerabilityTimer;

	void EnableRedTurretVulnerability(float Duration);

	void DisableRedTurretVulnerability();
protected:

	void SpawnRedDrone();
};