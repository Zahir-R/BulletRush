#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "Level2SFacade.generated.h"

class AMechaEnemyFactory;
class ADronMecha;
class ABatteryActor;
class ALevelPortal;

UCLASS()
class BULLETRUSH_API ALevel2SFacade : public AActor
{
    GENERATED_BODY()

public:
    ALevel2SFacade();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "Level2S|Spawn")
    TArray<FVector> DroneSpawnLocations;

    UPROPERTY(EditDefaultsOnly, Category = "Level2S|Config")
    float VenomDamagePerSecond = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Level2S|Config")
    float BatteryOffset = 150.0f;

    UPROPERTY(EditAnywhere, Category = "Level2S|References")
    ALevelPortal* PortalToBoss;

    void StartLevel();

    UFUNCTION()
    void OnDroneKilled(AEnemyBase* DeadEnemy);

private:
    UPROPERTY()
    AMechaEnemyFactory* Factory;

    UPROPERTY()
    TArray<ADronMecha*> ActiveDrones;

    int32 DronesKilled = 0;
    int32 TotalDrones = 15;
    bool bLevelComplete = false;

    FTimerHandle VenomTimer;

    void SpawnWave();
    void ApplyVenom();
    void CheckLevelComplete();
    void OpenPortal();

    UFUNCTION()
    void OnPlayerDeath();
};