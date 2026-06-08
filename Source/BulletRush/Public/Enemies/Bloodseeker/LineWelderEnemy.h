#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
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

private:
    UPROPERTY(EditAnywhere, Category = "Movement")
    float Speed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float StopDistance;

	
    UFUNCTION()
    void RotateTowardsPlayer(float DeltaTime);

    UPROPERTY(EditAnywhere, Category = "Movement")
    FRotator MeshRotationOffset = FRotator(0.0f, 90.0f, 0.0f);

    UPROPERTY(EditAnywhere, Category = "Movement")
    float RotationSpeed = 5.0f;
};
