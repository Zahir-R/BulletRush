#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "KamikazeEnemy.generated.h"

UCLASS()
class BULLETRUSH_API AKamikazeEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AKamikazeEnemy();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Die() override;

    void InitializeArc(const FVector& BossLocation, const FVector& PlayerLocation);

protected:
    UFUNCTION()
    void OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

private:
    UPROPERTY(EditAnywhere, Category = "Kamikaze | Movement", meta = (AllowPrivateAccess = "true"))
    float Speed;

    UPROPERTY(EditAnywhere, Category = "Kamikaze | Combat", meta = (AllowPrivateAccess = "true"))
    float ImpactDamage;

    bool bHasReachedTarget;
};
