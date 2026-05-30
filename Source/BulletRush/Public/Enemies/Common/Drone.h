#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "Drone.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API ADrone : public AEnemyBase
{
    GENERATED_BODY()
public:
    ADrone();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void StartAttack() override;

    void ApplySpeedBuff(float Duration, float FireRateMult, float ProjectileSpeedMult);
    void RemoveSpeedBuff();

protected:
    UPROPERTY()
    UMovementStrat* MovementStrategy;
    float ProjectileSpeed;
    float Damage;
    float CurrentProjectileSpeedMultiplier;
    // Members for speed buff restoration
    float OriginalAttackInterval;
    float OriginalProjectileSpeedMultiplier;
    FTimerHandle SpeedBuffRestoreTimerHandle;
};
