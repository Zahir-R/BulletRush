#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "LineWelderEnemy.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API ALineWelderEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    ALineWelderEnemy();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void StartAttack() override;
    virtual void Die() override;

    UFUNCTION()
    void OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

protected:
    UPROPERTY()
    UMovementStrat* MovementStrategy;

private:
};
