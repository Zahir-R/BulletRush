#pragma once
#include "CoreMinimal.h"
#include "ChronostasisSlowSystem.generated.h"

class AChronostasisFacade;

UCLASS()
class UChronostasisSlowSystem : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(AChronostasisFacade* Owner);
    void Start();
    void Stop();
    void ResetOnKill();
    int32 GetTriggerCount() const { return SlowTriggerCount; }

    FSimpleMulticastDelegate OnSlowTriggered;

private:
    void OnTimerExpired();
    void StartTimer();

    FTimerHandle SlowTimerHandle;
    int32 SlowTriggerCount;

    UPROPERTY()
    AChronostasisFacade* OwnerFacade;
};
