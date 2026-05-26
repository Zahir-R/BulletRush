#pragma once
#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "GameModeChronostasis.generated.h"

UCLASS()
class BULLETRUSH_API AGameModeChronostasis : public ABulletRushGameModeBase
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    UFUNCTION()
    void ActivateSecretPortal();

    UFUNCTION()
    void ActivateBossPortal();

    // Local portal actors that can be activated by the game mode
    UPROPERTY(EditAnywhere, Category="Portals")
    class APortalTrigger* SecretPortal;

    UPROPERTY(EditAnywhere, Category="Portals")
    class APortalTrigger* BossPortal;
};
