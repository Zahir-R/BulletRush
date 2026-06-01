#pragma once
#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "GameModeChronostasis.generated.h"

class URequirementManager;

UCLASS(Blueprintable)
class BULLETRUSH_API AGameModeChronostasis : public ABulletRushGameModeBase
{
    GENERATED_BODY()
public:
    AGameModeChronostasis();
    virtual void BeginPlay() override;

    UFUNCTION()
    void ActivateSecretPortal();

    UFUNCTION()
    void ActivateBossPortal();

    void StartSecretLevel(URequirementManager* RequirementManager);
    void OnSecretLevelCompleted();

    UPROPERTY(EditAnywhere, Category="Portals")
    FVector BossPortalSpawnLocation;

    // Template class for the boss portal
    UPROPERTY(EditAnywhere, Category="Portals")
    TSubclassOf<class APortalTrigger> BossPortalClass;

    // Location where the secret portal will spawn when requirements are met
    UPROPERTY(EditAnywhere, Category="Portals")
    FVector SecretPortalSpawnLocation = FVector(300.0f, 300.0f, 300.0f);

    // Template class for the secret portal, so designers can assign a BP with custom visuals
    UPROPERTY(EditAnywhere, Category="Portals")
    TSubclassOf<class APortalTrigger> SecretPortalClass;

protected:
    void OnSecretPortalTriggered();
    void OnBossPortalTriggered();

    UPROPERTY()
    AChronostasisFacade* CachedFacade;

    UPROPERTY()
    class APortalTrigger* SpawnedSecretPortal;

    UPROPERTY()
    class APortalTrigger* SpawnedBossPortal;

    TArray<FWaveConfig> SecretWaves;

	bool bSecretLevelTriggered = false;
	bool bBossPortalTriggered = false;
};
