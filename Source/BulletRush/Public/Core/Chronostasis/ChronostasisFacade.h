#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Chronostasis/ChronostasisEnemyFactory.h"
#include "ChronostasisFacade.generated.h"

class AEnemyBase;
class ABulletRushGameModeBase;

USTRUCT()
struct FWaveConfig
{
    GENERATED_BODY()
    UPROPERTY()
    int32 DroneCount = 0;
    UPROPERTY()
    int32 MassCount = 0;
    UPROPERTY()
    int32 ExpansiveCount = 0;
    UPROPERTY()
    TArray<FVector> SpawnPoints;
};

UCLASS()
class AChronostasisFacade : public AActor
{
    GENERATED_BODY()
public:
    AChronostasisFacade();
    ~AChronostasisFacade();
    virtual void BeginPlay() override;
    void StartGame();
    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* Enemy);
    int32 GetSlowTriggerCount() const { return SlowTriggerCount; }
    void ActivatePortalToSecret();
    void ActivatePortalToBoss();

    // Observer pattern: broadcast when a time stop occurs
    FSimpleMulticastDelegate OnTimeStop;

protected:
    UPROPERTY(EditAnywhere)
    TArray<FWaveConfig> Waves;

    int32 CurrentWaveIndex;
    int32 RemainingEnemiesInWave;
    int32 SlowTriggerCount;

    // Whether the player took damage during the session - used for secret unlock condition
    bool bPlayerTookDamage = false;

    // Weak pointer to the player's health component for damage observation
    TWeakObjectPtr<class UHealthComponent> PlayerHealthComp;

    TUniquePtr<IChronostasisEnemyFactory> Factory;

    FTimerHandle SlowTimerHandle;
    void StartSlowTimer();
    void OnSlowTimerExpired();
    UFUNCTION()
    void OnPlayerHealthChanged(float NewHealth);

    void StartWave(int32 Index);
    void OnAllWavesComplete();

    ABulletRushGameModeBase* OwningGameMode;
};
