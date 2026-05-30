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

    // Pre-placed boss portal in the level
    UPROPERTY(EditAnywhere, Category="Portals")
    class APortalTrigger* BossPortal;

    // Location where the secret portal will spawn when requirements are met
    UPROPERTY(EditAnywhere, Category="Portals")
    FVector SecretPortalSpawnLocation;

    // Template class for the secret portal, so designers can assign a BP with custom visuals
    UPROPERTY(EditAnywhere, Category="Portals")
    TSubclassOf<class APortalTrigger> SecretPortalClass;

protected:
    void OnSecretPortalTriggered();

    UPROPERTY()
    AChronostasisFacade* CachedFacade;

    UPROPERTY()
    class APortalTrigger* SpawnedSecretPortal;

    TArray<FWaveConfig> SecretWaves;

    bool bSecretLevelTriggered = false;
};
