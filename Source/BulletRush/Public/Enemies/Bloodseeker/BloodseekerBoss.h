
#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "BloodseekerBoss.generated.h"

class APlayingPlayer;
class AKamikazeEnemy;
class ABloodseekerFacade;


// Enumerador de fases de ataque del Boss

UENUM(BlueprintType)
enum class EBossAttackState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    LinearBurst         UMETA(DisplayName = "Linear Burst"),
    RadialSphere        UMETA(DisplayName = "Radial Sphere"),
    UltimateMalediction UMETA(DisplayName = "Ultimate Malediction")
};


// Clase principal del Boss Bloodseeker

UCLASS()
class BULLETRUSH_API ABloodseekerBoss : public ABossBase
{
    GENERATED_BODY()

public:
    ABloodseekerBoss();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Attack() override;
    virtual void Die() override;

    // Referencia al Facade para pausar/reanudar boss waves asignada desde GameMode
    UPROPERTY()
    ABloodseekerFacade* FacadeRef;

protected:

private:
  
    // Referencia al jugador
    UPROPERTY()
    APlayingPlayer* TargetPlayer;

  
    // Rotacion matematica hacia el jugador 
    void RotateTowardsPlayer(float DeltaTime);

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rotation", meta = (AllowPrivateAccess = "true"))
    float RotationSpeed;

 
    // Ciclo de ataques ,sub-estados internos
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bloodseeker | Combat", meta = (AllowPrivateAccess = "true"))
    EBossAttackState CurrentAttackState;

    void ExecuteLinearBurst();
    void ExecuteRadialSphere();
    void ExecuteUltimateMalediction();
    void CycleNextAttack();
    int32 AttackCycleIndex;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Combat", meta = (AllowPrivateAccess = "true"))
    float TimeBetweenAttacks;

    FTimerHandle AttackCycleTimer;


    // Mecanica Rupture  daño por movimiento ,3 ejes

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    bool bIsRuptureActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    bool bIsMaledictionActive;

    FVector LastPlayerLocation;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    float RuptureDamageMultiplier;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rupture", meta = (AllowPrivateAccess = "true"))
    float RuptureDuration;

    FTimerHandle RuptureTimerHandle;

    // Oleadas de Kamikazes
    void SpawnKamikazeWave(int32 Count, float DelayBetween);
    FTimerHandle WaveTimerHandle;
    FTimerHandle WaveTimerHandle2;
    FTimerHandle KamikazeWaveTimer;
    FTimerHandle KamikazeWaveTimer2;

    void ActivateMalediction();
    void DeactivateMalediction();

    void OnBossHealthChanged(float NewHealth);

    // Ultimate por tiempo
    int32 UltimateCount;
    FTimerHandle UltimateTimerHandle;
    void TryActivateUltimate();

    // Movimiento sinusoidal en la arena
 
    FVector InitialLocation;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Movement", meta = (AllowPrivateAccess = "true"))
    float MovementAmplitude;

    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Movement", meta = (AllowPrivateAccess = "true"))
    float MovementFrequency;

    // Combos de disparo inyectados al BulletSpawner

    TArray<FAttackStep> LinearBurstCombo;
    TArray<FAttackStep> RadialCombo;
    TArray<FAttackStep> AttackSequence;
};