#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "Level21Facade.generated.h"

class AMechaEnemyFactory;
class ADronMecha;
class ABatteryActor;
class ALevelPortal;

UCLASS()
class BULLETRUSH_API ALevel21Facade : public AActor
{
    GENERATED_BODY()

public:
    ALevel21Facade();

protected:
    virtual void BeginPlay() override;

public:
    // --- Configuracion ---
    UPROPERTY(EditAnywhere, Category = "Level21|Spawn")
    TArray<FVector> DroneSpawnLocations;

    UPROPERTY(EditDefaultsOnly, Category = "Level21|Config")
    float LevelTimeLimit = 180.0f; // 3 minutos

    UPROPERTY(EditDefaultsOnly, Category = "Level21|Config")
    float BatteryOffset = 150.0f; // distancia bateria respecto al drone

    // --- Referencias de nivel ---
    UPROPERTY(EditAnywhere, Category = "Level21|References")
    ALevelPortal* PortalToSecret;

    UPROPERTY(EditAnywhere, Category = "Level21|References")
    ALevelPortal* PortalToBoss;

    // --- API publica ---
    void StartLevel();

    UFUNCTION()
    void OnDroneKilled(AEnemyBase* DeadEnemy);

private:
    UPROPERTY()
    AMechaEnemyFactory* Factory;

    UPROPERTY()
    TArray<ADronMecha*> ActiveDrones;

    int32 DronesKilled = 0;
    int32 TotalDrones = 1;
    bool bPlayerDied = false;
    bool bLevelComplete = false;

    FTimerHandle LevelTimer;

    void SpawnWave();
    void OnTimerExpired();
    void CheckLevelComplete();
    void OpenPortal(bool bToSecret);

    // Para detectar si el jugador murio
    UFUNCTION()
    void OnPlayerDeath();
};