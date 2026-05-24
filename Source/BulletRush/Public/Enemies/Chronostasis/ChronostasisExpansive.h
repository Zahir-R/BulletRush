#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy.h"
#include "ChronostasisExpansive.generated.h"

UCLASS()
class AChronostasisExpansive : public AEnemyBase
{
    GENERATED_BODY()
public:
    AChronostasisExpansive();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    FTimerHandle PulseTimerHandle;
    void TriggerPulse();
};
