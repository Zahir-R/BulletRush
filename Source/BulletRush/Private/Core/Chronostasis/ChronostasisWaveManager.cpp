#include "Core/Chronostasis/ChronostasisWaveManager.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Chronostasis/DroneFactory.h"
#include "Core/Chronostasis/MassFactory.h"
#include "Core/Chronostasis/ExpansiveFactory.h"
#include "Core/Chronostasis/ChargerFactory.h"
#include "Core/Chronostasis/LinkerFactory.h"
#include "Enemies/EnemyBase.h"
#include "Engine/World.h"

void UChronostasisWaveManager::Initialize(AChronostasisFacade* Owner, const TArray<FWaveConfig>& DefaultWaves)
{
    OwnerFacade = Owner;
    Waves = DefaultWaves;
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;
    CreateFactories();
}

void UChronostasisWaveManager::CreateFactories()
{
    DroneFactory = NewObject<UDroneFactory>(this);
    MassFactory = NewObject<UMassFactory>(this);
    ExpansiveFactory = NewObject<UExpansiveFactory>(this);
    ChargerFactory = NewObject<UChargerFactory>(this);
    LinkerFactory = NewObject<ULinkerFactory>(this);
}

void UChronostasisWaveManager::StartGame()
{
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;

    if (Waves.Num() == 0)
    {
        FWaveConfig W1; W1.DroneCount = 1; W1.SpawnPoints = { FVector(1000,0,0), FVector(-1000,0,0), FVector(0,1000,0) };
        FWaveConfig W2; W2.DroneCount = 1; W2.MassCount = 1; W2.SpawnPoints = { FVector(200,0,0), FVector(-200,0,0), FVector(0,200,0) };
        FWaveConfig W3; W3.DroneCount = 1; W3.MassCount = 1; W3.ExpansiveCount = 1; W3.SpawnPoints = { FVector(300,0,0), FVector(-300,0,0), FVector(0,300,0) };
        Waves = { W1, W2, W3 };
    }

    StartWave(0);
}

void UChronostasisWaveManager::StartSecretWaves(const TArray<FWaveConfig>& NewWaves)
{
    Waves = NewWaves;
    CurrentWaveIndex = 0;
    StartWave(0);
}

void UChronostasisWaveManager::StartWave(int32 Index)
{
    if (!Waves.IsValidIndex(Index))
    {
        return;
    }

    CurrentWaveIndex = Index;
    const FWaveConfig& Cfg = Waves[Index];

    UWorld* World = OwnerFacade ? OwnerFacade->GetWorld() : nullptr;
    if (!World) return;

    int32 SpawnedCount = 0;
    int32 SpawnIndex = 0;

    auto TrySpawn = [&](int32 Count, UChronostasisFactoryEnemy* Factory)
    {
        for (int32 i = 0; i < Count && SpawnIndex < Cfg.SpawnPoints.Num(); i++)
        {
            FVector Loc = Cfg.SpawnPoints[SpawnIndex++];
            AEnemyBase* E = Factory->CreateEnemy(World, Loc);
            if (E)
            {
                E->OnEnemyDeath.AddDynamic(this, &UChronostasisWaveManager::OnEnemyKilled);
                SpawnedCount++;
            }
        }
    };

    TrySpawn(Cfg.DroneCount, DroneFactory);
    TrySpawn(Cfg.MassCount, MassFactory);
    TrySpawn(Cfg.ExpansiveCount, ExpansiveFactory);
    TrySpawn(Cfg.ChargerCount, ChargerFactory);
    TrySpawn(Cfg.LinkerCount, LinkerFactory);

    RemainingEnemiesInWave = SpawnedCount;
}

void UChronostasisWaveManager::OnEnemyKilled(AEnemyBase* Enemy)
{
    if (RemainingEnemiesInWave <= 0) return;
    RemainingEnemiesInWave--;

    OnWaveEnemyKilled.Broadcast(Enemy);

    if (RemainingEnemiesInWave == 0)
    {
        if (CurrentWaveIndex + 1 < Waves.Num())
        {
            StartWave(CurrentWaveIndex + 1);
        }
        else
        {
            OnAllWavesCompleted.Broadcast();
        }
    }
}

bool UChronostasisWaveManager::AreAllWavesComplete() const
{
    return RemainingEnemiesInWave <= 0 && CurrentWaveIndex >= Waves.Num() - 1;
}
