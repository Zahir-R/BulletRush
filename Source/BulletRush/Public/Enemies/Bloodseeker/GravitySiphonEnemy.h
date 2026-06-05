#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "GravitySiphonEnemy.generated.h"

class USphereComponent;

UCLASS()
class BULLETRUSH_API AGravitySiphonEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AGravitySiphonEnemy();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void StartAttack() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* SuctionVolume;

    UPROPERTY(EditAnywhere, Category = "GravitySiphon | Suction", meta = (AllowPrivateAccess = "true"))
    float PullForce;

    UPROPERTY(EditAnywhere, Category = "GravitySiphon | Suction", meta = (AllowPrivateAccess = "true"))
    float DamagePerSecond;

    UPROPERTY(EditAnywhere, Category = "GravitySiphon | Suction", meta = (AllowPrivateAccess = "true"))
    float StillnessThreshold;

    UPROPERTY(EditAnywhere, Category = "GravitySiphon | Suction", meta = (AllowPrivateAccess = "true"))
    float ActivationRadius;
};
