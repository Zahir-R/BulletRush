#pragma once

#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "BloodseekerGameMode.generated.h"

class ABloodseekerFacade;
class ABloodseekerBoss;
class ABossSpawnTrigger;
class ALevelPortal;
class AEnemyBase;
class ASkySphereWorld;

class APlayingPlayer;
class USoundBase;

UCLASS()
class BULLETRUSH_API ABloodseekerGameMode : public ABulletRushGameModeBase
{
    GENERATED_BODY()

public:
    ABloodseekerGameMode();

    virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION()
    void OnAllWavesComplete();

    UFUNCTION()
    void OnBossDied(AEnemyBase* DeadBoss);

    UFUNCTION()
    void OnBossSpawned(ABloodseekerBoss* SpawnedBoss);

    UFUNCTION()
    void OnPlayerDeath();

protected:
    UPROPERTY()
    ABloodseekerFacade* Facade;

    UPROPERTY()
    ABloodseekerBoss* BossRef;

    UPROPERTY()
    ABossSpawnTrigger* BossTrigger;
    
    UPROPERTY()
	ASkySphereWorld* SkySphere;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker Level")
    TSubclassOf<ABloodseekerFacade> FacadeClass;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker Level")
    TSubclassOf<ABossSpawnTrigger> BossTriggerClass;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker Level")
    TSubclassOf<ALevelPortal> PortalClass;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker Level")
    FVector SpawnBossTriggerLocation;

private:
    void CleanupLevel();

public:
    UPROPERTY(EditAnywhere, Category = "Music")
    USoundBase* AmbientSong;

    bool bBossDied;
};
