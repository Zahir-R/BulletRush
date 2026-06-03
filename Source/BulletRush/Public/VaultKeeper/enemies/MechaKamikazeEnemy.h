#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "MechaKamikazeEnemy.generated.h"

/**
 * MechaKamikazeEnemy
 *
 * Dron suicida de la facción Mecha-Supremacy.
 * Basado en AKamikazeEnemy (Bloodseeker) pero independiente.
 *
 * Diferencias vs Kamikaze original:
 *   - Más rápido (850 vs 600)
 *   - Más daño al impacto (15 vs 5)
 *   - Sale spawneado cuando la antena de su zona es destruida:
 *     la Facade lo spawnea en OnAntennaDestroyed() como respuesta
 *     de "último recurso" del área sin cobertura.
 */
UCLASS(Blueprintable)
class BULLETRUSH_API AMechaKamikazeEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AMechaKamikazeEnemy();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Die() override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "MechaKamikaze|Movement")
    float Speed = 250.0f;

    UPROPERTY(EditDefaultsOnly, Category = "MechaKamikaze|Combat")
    float ImpactDamage = 55.0f;

private:
    bool bHasReachedTarget = false;

    UFUNCTION()
    void OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};