#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Combat/MovementStrategy/MovementStrat.h"
#include "MechaChargerEnemy.generated.h"

/**
 * MechaChargerEnemy
 *
 * Enemigo de la facción Mecha-Supremacy que se lanza contra el jugador.
 * Basado en el comportamiento de AChronostasisCharger pero completamente
 * independiente — no toca código de otro nivel.
 *
 * Diferencia clave vs ChronostasisCharger:
 *   Al impactar aplica PlayerSlowDecorator (ralentiza) en vez de
 *   ParalysisDecorator (paraliza). Temáticamente: un mecha pesado que
 *   te aplasta y te deja lento, haciéndote más vulnerable a los drones.
 */
UCLASS(Blueprintable)
class BULLETRUSH_API AMechaChargerEnemy : public AEnemyBase
{
    GENERATED_BODY()

public:
    AMechaChargerEnemy();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "MechaCharger|Movement")
    float ChargeSpeed = 1500.0f;

    // Cuánto dura el slow que aplica al jugador al impactar
    UPROPERTY(EditDefaultsOnly, Category = "MechaCharger|Combat")
    float SlowDuration = 3.5f;

    UPROPERTY()
    UMovementStrat* MovementStrategy;

private:
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* HitCollision;

    UFUNCTION()
    void OnHitPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};