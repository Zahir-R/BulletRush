#pragma once
#include "CoreMinimal.h"
#include "ChronostasisSecretManager.generated.h"

class AChronostasisFacade;

UCLASS()
class UChronostasisSecretManager : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AChronostasisFacade* Owner);

    void Start();
    void ClearAllTimers();
    void Reset();

    bool IsActive() const { return bIsActive; }

    FSimpleMulticastDelegate OnSecretTimeUp;

private:
    void TickTimer();
    void OnTimeUpTeleport();

    bool bIsActive = false;
    float TimeRemaining = 120.f;
    bool bTimerPaused = false;
    FTimerHandle CountdownTimerHandle;
    FTimerHandle TeleportDelayHandle;

    UPROPERTY()
    AChronostasisFacade* OwnerFacade;
};
