#pragma once
#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "WaveConfig.h"
#include "ChronostasisFacade.generated.h"

class AEnemyBase;
class ABulletRushGameModeBase;
class ABulletRushHUD;
class ASerXBoss;
class UChronostasisWaveManager;
class UChronostasisSlowSystem;
class UChronostasisBossManager;
class UChronostasisSecretManager;
class URequirementManager;
class UHealthComponent;

UCLASS()
class AChronostasisFacade : public APublisher
{
    GENERATED_BODY()
public:
    AChronostasisFacade();
    ~AChronostasisFacade();
    virtual void BeginPlay() override;

    void StartGame();
    UFUNCTION()
    void OnEnemyKilled(AEnemyBase* Enemy);

    int32 GetSlowTriggerCount() const;

    void SetRequirementManager(class URequirementManager* Manager);
    void StartSecretWaves(const TArray<FWaveConfig>& NewWaves);

    bool AreAllWavesComplete() const;

    void StartBossFight();
    UFUNCTION()
    void OnBossKilled(AEnemyBase* Boss);

    UFUNCTION()
    void OnBossPortalTriggered();

    void OnSecretTimeUp();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Sub-Managers")
    UChronostasisWaveManager* WaveManager;

    UPROPERTY(VisibleAnywhere, Category = "Sub-Managers")
    UChronostasisSlowSystem* SlowSystem;

    UPROPERTY(VisibleAnywhere, Category = "Sub-Managers")
    UChronostasisBossManager* BossManager;

    UPROPERTY(VisibleAnywhere, Category = "Sub-Managers")
    UChronostasisSecretManager* SecretManager;

    UPROPERTY(EditAnywhere, Category = "Waves")
    TArray<FWaveConfig> Waves;

    UPROPERTY(EditAnywhere, Category = "Boss")
    TSubclassOf<ASerXBoss> SerXBossClass;

    UPROPERTY(EditAnywhere, Category = "Boss")
    FVector BossArenaSpawnLocation;

    UPROPERTY()
    TWeakObjectPtr<URequirementManager> RequirementManagerRef;

    bool bPlayerTookDamage = false;
    TWeakObjectPtr<UHealthComponent> PlayerHealthComp;

    ABulletRushGameModeBase* OwningGameMode;

    ABulletRushHUD* GetHUD() const;

    UFUNCTION()
    void OnPlayerHealthChanged(float NewHealth);

    void OnAllWavesComplete();
};
