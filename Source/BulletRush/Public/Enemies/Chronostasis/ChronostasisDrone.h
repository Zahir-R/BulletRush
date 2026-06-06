#pragma once
#include "CoreMinimal.h"
#include "Enemies/Common/Drone.h"
#include "ChronostasisDrone.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API AChronostasisDrone : public ADrone
{
    GENERATED_BODY()
public:
    AChronostasisDrone();
    virtual void StartAttack() override;

    void ApplySpeedBuff(float Duration, float FireRateMult, float ProjectileSpeedMult);

protected:
    void RemoveSpeedBuff();

    bool bSpeedBuffBaselineSaved = false;
};
