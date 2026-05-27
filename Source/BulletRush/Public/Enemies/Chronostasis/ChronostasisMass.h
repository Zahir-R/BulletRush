#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "ChronostasisMass.generated.h"

class USphereComponent;
class UBuffComponent;
class UPlayerStatsDecorator;

UCLASS()
class AChronostasisMass : public AEnemyBase
{
    GENERATED_BODY()
public:
    AChronostasisMass();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    UPROPERTY()
    USphereComponent* SlowZone;
    UPROPERTY()
    UMovementStrat* MovementStrategy;
    UPlayerStatsDecorator* AppliedDecorator;

    UFUNCTION()
    void OnSlowZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    UFUNCTION()
    void OnSlowZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    bool bHasAppliedBuff = false;
};
