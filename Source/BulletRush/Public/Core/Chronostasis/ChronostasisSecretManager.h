#pragma once
#include "CoreMinimal.h"
#include "ChronostasisSecretManager.generated.h"

UCLASS()
class UChronostasisSecretManager : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AActor* Owner);

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
    AActor* OwnerActor;
};
