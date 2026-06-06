
#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "BloodseekerBoss.generated.h"

class APlayingPlayer;
class AKamikazeEnemy;
class ABloodseekerFacade;
class UBossStateUltimate;


// Enumerador de fases de ataque del Boss

// NOTA: Diseño inicial para sub-estados de ataque.
// Idle y LinearBurst se usan en Attack() para la primera llamada.
// RadialSphere aparece en el switch pero nunca se asigna manualmente.
// UltimateMalediction no se usa (Ultimate se controla desde BossStateUltimate).
// El ciclo real de ataques lo controla AttackCycleIndex en CycleNextAttack().
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

    friend class UBossStateUltimate;

public:
    ABloodseekerBoss();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Attack() override;
    virtual void Die() override;
    virtual void ChangeState(UBossState* NewState) override;

    // Estado Ultimate (Maldición del Sangrado)
    UPROPERTY()
    UBossStateUltimate* UltimateState;

    // Timer de transición de Attacking → Ultimate (20s)
    FTimerHandle UltimateTransitionTimer;

    // Referencia al Facade para pausar/reanudar boss waves asignada desde GameMode
    UPROPERTY()
    ABloodseekerFacade* FacadeRef;
   
    UPROPERTY(EditAnywhere, Category = "Bloodseeker | Rotation")
    FRotator MeshRotationOffset;

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
    // NOTA: Solo se usa en la primera llamada a Attack(). El ciclo continuo
    // lo controla AttackCycleIndex en CycleNextAttack().
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bloodseeker | Combat", meta = (AllowPrivateAccess = "true"))
    EBossAttackState CurrentAttackState;

    void ExecuteLinearBurst();
    void ExecuteRadialSphere();
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

    // Oleadas de Kamikazes
    void SpawnKamikazeWave(int32 Count, float DelayBetween);

    void OnBossHealthChanged(float NewHealth);

    int32 UltimateCount;

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