#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy.h"
#include "Drone.generated.h"

UCLASS()
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
    TSharedPtr<IMovementStrategy> MovementStrategy;
    float ProjectileSpeed;
    float Damage;
    float CurrentProjectileSpeedMultiplier;
};
