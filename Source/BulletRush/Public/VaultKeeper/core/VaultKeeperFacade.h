#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "VaultKeeperFacade.generated.h"

class AMechaEnemyFactory;
class AVaultKeeper;
class ADronMecha;
class ABatteryActor;
class ALevelPortal;
class UBulletRushGameInstance;

UCLASS()
class BULLETRUSH_API AVaultKeeperFacade : public AActor
{
    GENERATED_BODY()

public:
    AVaultKeeperFacade();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "VaultKeeperFacade|Spawn")
    FVector BossSpawnLocation = FVector(1200.f, 600.f, 1300.f);

    UPROPERTY(EditAnywhere, Category = "VaultKeeperFacade|Spawn")
    TArray<FVector> DroneSpawnLocations;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeperFacade|Config")
    float DroneSpawnInterval = 30.0f;

    UPROPERTY(EditDefaultsOnly, Category = "VaultKeeperFacade|Config")
    int32 DronesPerSpawn = 2;

    UPROPERTY(EditAnywhere, Category = "VaultKeeperFacade|References")
    ALevelPortal* PortalToMap;

    void StartLevel();

    UFUNCTION()
    void OnBossDeath(AEnemyBase* DeadEnemy);

    UFUNCTION()
    void OnDroneKilled(AEnemyBase* DeadEnemy);

private:
    UPROPERTY()
    AMechaEnemyFactory* Factory;

    UPROPERTY()
    AVaultKeeper* BossInstance;

    UPROPERTY()
    TArray<AEnemyBase*> ActiveEnemies;

    int32 DroneSpawnCount = 0;
    bool bLevelComplete = false;

    FTimerHandle DroneSpawnTimer;

    void SpawnBoss();
    void SpawnDroneWave();
    void OpenPortal();
};