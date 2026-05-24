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
};
