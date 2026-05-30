#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "UObject/Interface.h"
#include "ChronostasisFacade.generated.h"

class AEnemyBase;
class ABulletRushGameModeBase;
class ABulletRushHUD;
class UChronostasisFactoryEnemy;

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
    int32 ChargerCount = 0;
    UPROPERTY()
    int32 LinkerCount = 0;
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

    void SetRequirementManager(class URequirementManager* Manager);
    void StartSecretWaves(const TArray<FWaveConfig>& NewWaves);

    bool AreAllWavesComplete() const { return RemainingEnemiesInWave <= 0 && CurrentWaveIndex >= Waves.Num() - 1; }

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	UChronostasisFactoryEnemy* DroneFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	UChronostasisFactoryEnemy* MassFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	UChronostasisFactoryEnemy* ExpansiveFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	UChronostasisFactoryEnemy* ChargerFactory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Factory")
	UChronostasisFactoryEnemy* LinkerFactory;

    FTimerHandle SlowTimerHandle;
    ABulletRushHUD* GetHUD() const;

    void StartSlowTimer();
    void OnSlowTimerExpired();
    UFUNCTION()
    void OnPlayerHealthChanged(float NewHealth);

    void StartWave(int32 Index);
    void OnAllWavesComplete();

    // Secret level support
    bool bIsSecretLevel = false;
    float SecretLevelTimeRemaining = 120.f;
    bool bSecretTimerPaused = false;
    FTimerHandle SecretCountdownTimerHandle;
    FTimerHandle SecretTeleportDelayHandle;

    void OnSecretCountdownTick();
    void OnSecretTimeUpTeleport();

    UPROPERTY()
    TWeakObjectPtr<class URequirementManager> RequirementManagerRef;

    ABulletRushGameModeBase* OwningGameMode;
};
