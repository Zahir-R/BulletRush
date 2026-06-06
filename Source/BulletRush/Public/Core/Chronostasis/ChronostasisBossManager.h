#pragma once
#include "CoreMinimal.h"
#include "ChronostasisFactoryEnemy.h"
#include "ChronostasisBossManager.generated.h"

class AEnemyBase;
class ASerXBoss;

UCLASS()
class UChronostasisBossManager : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AActor* Owner, TSubclassOf<ASerXBoss> BossClass, const FVector& SpawnLocation);

    void StartBossFight();
    UFUNCTION()
    void OnBossKilled(AEnemyBase* Boss);

    void SetLinkerFactory(UChronostasisFactoryEnemy* Factory) { BossLinkerFactory = Factory; }

    FSimpleMulticastDelegate OnBossDefeated;

private:
    UPROPERTY()
    TSubclassOf<ASerXBoss> SerXBossClass;

    FVector BossArenaSpawnLocation;

    UPROPERTY()
    UChronostasisFactoryEnemy* BossLinkerFactory;
    UPROPERTY()
    TArray<UChronostasisFactoryEnemy*> MinionFactories;

    UPROPERTY()
    AActor* OwnerActor;
};
