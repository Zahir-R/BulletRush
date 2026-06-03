#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemies/EnemyBase.h"
#include "Level2SFacade.generated.h"

class AMechaEnemyFactory;
class ADronMecha;
class AMechaChargerEnemy;
class AMechaKamikazeEnemy;
class ABatteryActor;
class ALevelPortal;

USTRUCT()
struct FHiveData
{
    GENERATED_BODY()

    UPROPERTY()
    ABatteryActor* Battery = nullptr;

    UPROPERTY()
    TArray<AEnemyBase*> Enemies;

    bool IsCleared() const { return Enemies.Num() == 0; }
};

UCLASS()
class BULLETRUSH_API ALevel2SFacade : public AActor
{
    GENERATED_BODY()

public:
    ALevel2SFacade();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Level2S|Config")
    float VenomDamagePerSecond = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Level2S|References")
    ALevelPortal* PortalToBoss;

    void StartLevel();

    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* DeadEnemy);

private:
    UPROPERTY()
    AMechaEnemyFactory* Factory;

    UPROPERTY()
    TArray<FHiveData> Hives;

    int32 HivesCleared = 0;
    bool bLevelComplete = false;

    FTimerHandle VenomTimer;

    void SpawnHives();
    void SpawnHive(int32 HiveIndex, FVector HiveCenter);
    void ApplyVenom();
    void CheckLevelComplete();
    void OpenPortal();

    UFUNCTION()
    void OnPlayerDeath();
};