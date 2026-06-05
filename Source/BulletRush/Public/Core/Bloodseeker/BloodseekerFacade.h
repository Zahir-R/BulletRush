#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Bloodseeker/BloodseekerEnemyFactoryBase.h"
#include "Core/Bloodseeker/KamikazeFactory.h"
#include "Core/Bloodseeker/LineWelderFactory.h"
#include "Core/Bloodseeker/GravitySiphonFactory.h"
#include "BloodseekerFacade.generated.h"

class AEnemyBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCompleteSignature);

USTRUCT(BlueprintType)
struct FBloodseekerWaveConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 KamikazeCount;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 LineWelderCount;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 GravitySiphonCount;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float DelayBetweenSpawns;

    FBloodseekerWaveConfig()
        : KamikazeCount(0), LineWelderCount(0), GravitySiphonCount(0), DelayBetweenSpawns(0.5f)
    {}
};

UCLASS()
class BULLETRUSH_API ABloodseekerFacade : public AActor
{
    GENERATED_BODY()

public:
    ABloodseekerFacade();

    void StartGame();
    void StartBossPhase();
    void PauseBossWaves();
    void ResumeBossWaves();
    void StopAllSpawning();
    void DestroyAllEnemies();

    FOnAllWavesCompleteSignature OnAllWavesComplete;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* DeadEnemy);

private:
    void SetupDefaultWaves();
    void SetupDefaultBossWaves();
    void StartWave(int32 Index);
    void StartTimerBossWave();
    void SpawnEnemiesForConfig(const FBloodseekerWaveConfig& Config);
    void SpawnSingleKamikaze(const FVector& Location);
    void SpawnSingleLineWelder(const FVector& Location);
    void SpawnSingleGravitySiphon(const FVector& Location);
    FVector GetRandomSpawnLocation();

    UPROPERTY()
    UKamikazeFactory* KamikazeFactory;

    UPROPERTY()
    ULineWelderFactory* LineWelderFactory;

    UPROPERTY()
    UGravitySiphonFactory* GravitySiphonFactory;

    UPROPERTY()
    TArray<FBloodseekerWaveConfig> HordasWaves;

    UPROPERTY()
    TArray<FBloodseekerWaveConfig> BossWaves;

    int32 CurrentWaveIndex;
    int32 RemainingEnemiesInWave;
    bool bIsBossPhase;

    FTimerHandle WaveTimerHandle;
    FTimerHandle BossWaveTimerHandle;
    FTimerHandle SpawnTimerHandle;
    TArray<FTimerHandle> SpawnTimerHandles;
};
