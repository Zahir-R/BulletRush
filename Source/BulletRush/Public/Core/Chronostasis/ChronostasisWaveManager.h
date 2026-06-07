#pragma once
#include "CoreMinimal.h"
#include "WaveConfig.h"
#include "ChronostasisFactoryEnemy.h"
#include "ChronostasisWaveManager.generated.h"

class AEnemyBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWaveEnemyKilled, AEnemyBase*);

UCLASS()
class UChronostasisWaveManager : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AActor* Owner, const TArray<FWaveConfig>& DefaultWaves);

    void StartGame();
    void StartSecretWaves(const TArray<FWaveConfig>& NewWaves);

    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* Enemy);

    bool AreAllWavesComplete() const;
    int32 GetRemainingEnemies() const { return RemainingEnemiesInWave; }
    int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }

    void SetWaves(const TArray<FWaveConfig>& NewWaves) { Waves = NewWaves; }

    FOnWaveEnemyKilled OnWaveEnemyKilled;
    FSimpleMulticastDelegate OnAllWavesCompleted;

private:
    void StartWave(int32 Index);
    void CreateFactories();

    UPROPERTY()
    TArray<FWaveConfig> Waves;

    int32 CurrentWaveIndex;
    int32 RemainingEnemiesInWave;

    UPROPERTY()
    AActor* OwnerActor;

    UPROPERTY()
    UChronostasisFactoryEnemy* DroneFactory;
    UPROPERTY()
    UChronostasisFactoryEnemy* MassFactory;
    UPROPERTY()
    UChronostasisFactoryEnemy* ExpansiveFactory;
    UPROPERTY()
    UChronostasisFactoryEnemy* ChargerFactory;
    UPROPERTY()
    UChronostasisFactoryEnemy* LinkerFactory;
};
