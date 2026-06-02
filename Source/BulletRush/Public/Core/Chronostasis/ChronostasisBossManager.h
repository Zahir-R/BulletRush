#pragma once
#include "CoreMinimal.h"
#include "ChronostasisFactoryEnemy.h"
#include "ChronostasisBossManager.generated.h"

class AEnemyBase;
class ASerXBoss;
class AChronostasisFacade;

UCLASS()
class UChronostasisBossManager : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AChronostasisFacade* Owner, TSubclassOf<ASerXBoss> BossClass, const FVector& SpawnLocation);

    void StartBossFight();
    UFUNCTION()
    void OnBossKilled(AEnemyBase* Boss);

    void SetChargerFactory(UChronostasisFactoryEnemy* Factory) { BossChargerFactory = Factory; }
    void SetLinkerFactory(UChronostasisFactoryEnemy* Factory) { BossLinkerFactory = Factory; }

    FSimpleMulticastDelegate OnBossDefeated;

private:
    UPROPERTY()
    TSubclassOf<ASerXBoss> SerXBossClass;

    FVector BossArenaSpawnLocation;

    UPROPERTY()
    UChronostasisFactoryEnemy* BossChargerFactory;
    UPROPERTY()
    UChronostasisFactoryEnemy* BossLinkerFactory;
    UPROPERTY()
    TArray<UChronostasisFactoryEnemy*> MinionFactories;

    UPROPERTY()
    AChronostasisFacade* OwnerFacade;
};
