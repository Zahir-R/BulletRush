#include "Core/Chronostasis/ChronostasisWaveManager.h"
#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UChronostasisWaveManager::Initialize(AActor* Owner, const TArray<FWaveConfig>& DefaultWaves)
{
    OwnerActor = Owner;
    Waves = DefaultWaves;
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;
    CreateFactories();
}

void UChronostasisWaveManager::CreateFactories()
{
    DroneFactory = CreateGenericFactory<AChronostasisDrone>(this);
    MassFactory = CreateGenericFactory<AChronostasisMass>(this);
    ExpansiveFactory = CreateGenericFactory<AChronostasisExpansive>(this);
    ChargerFactory = CreateGenericFactory<AChronostasisCharger>(this);
    LinkerFactory = CreateGenericFactory<AChronostasisLinker>(this);
}

void UChronostasisWaveManager::StartGame()
{
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;

    if (Waves.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChronostasisWaveManager::StartGame: No waves configured!"));
        return;
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

    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    if (!World) return;

    // Compute 5 pentagram points centred on the player
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    FVector Center = PlayerPawn ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;

    const float Radius = 800.0f;
    TArray<FVector> PentagramPoints;
    for (int32 i = 0; i < 5; i++)
    {
        float Angle = FMath::DegreesToRadians(72.0f * i);
        PentagramPoints.Add(Center + FVector(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), 0));
    }

    int32 SpawnedCount = 0;
    int32 PointIndex = 0;

    auto TrySpawn = [&](int32 Count, UChronostasisFactoryEnemy* Factory)
    {
        for (int32 i = 0; i < Count && PointIndex < PentagramPoints.Num(); i++)
        {
            FVector Loc = PentagramPoints[PointIndex++];
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
