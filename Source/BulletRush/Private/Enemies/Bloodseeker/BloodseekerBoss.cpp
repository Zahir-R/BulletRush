
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Player/PlayingPlayer.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABloodseekerBoss::ABloodseekerBoss()
{
    PrimaryActorTick.bCanEverTick = true;

    // Movimiento sinusoidal
    MovementAmplitude = 350.0f;
    MovementFrequency = 1.5f;

    // Rotación
    RotationSpeed = 10.0f;

    // Control de fases de ataque
    CurrentAttackState = EBossAttackState::Idle;
    TimeBetweenAttacks = 3.0f;

    // Rupture / Malediction
    bIsRuptureActive = false;
    bIsMaledictionActive = false;
    RuptureDamageMultiplier = 0.15f;
    RuptureDuration = 6.0f;
    LastPlayerLocation = FVector::ZeroVector;

    TargetPlayer = nullptr;
}

void ABloodseekerBoss::BeginPlay()
{
    Super::BeginPlay();

    InitialLocation = GetActorLocation();

    AActor* FoundPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (FoundPlayer)
    {
        TargetPlayer = Cast<APlayingPlayer>(FoundPlayer);
    }

    // Ráfaga lineal múltiple
    LinearBurstCombo.Add(FAttackStep(EAttackType::Burst, 8, 700.0f, 0.1f, 0.05f));

    // Explosión radial 360°
    RadialCombo.Add(FAttackStep(EAttackType::Sphere, 1000, 800.0f, 0.5f, 0.1f));
    // Ultimate: Proyectiles pesados envolventes de la Maldición
    //UltimateCombo.Add(FAttackStep(EAttackType::Burst, 12, 500.0f, 0.2f, 0.1f)); // Ajustable según diseño
}

void ABloodseekerBoss::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetPlayer || GetCurrentBossStateName() == "Dead") return;

    // ------- Rotación suave hacia el jugador -------
    RotateTowardsPlayer(DeltaTime);

    // ------- Movimiento sinusoidal (si no está stuneado) -------
    if (GetCurrentBossStateName() != "Stunned")
    {
        FVector NewLocation = InitialLocation;
        float TimeSecs = GetWorld()->GetTimeSeconds();
        NewLocation += GetActorRightVector()
            * FMath::Sin(TimeSecs * MovementFrequency)
            * MovementAmplitude;
        SetActorLocation(NewLocation);
    }

    // ------- Cálculo de sangrado Rupture (3 ejes) -------
    if (bIsRuptureActive)
    {
        FVector CurrentPlayerLocation = TargetPlayer->GetActorLocation();
        float DistanceMoved = FVector::Distance(CurrentPlayerLocation, LastPlayerLocation);

        if (DistanceMoved > 0.1f)
        {
            float CalculatedDamage = DistanceMoved * RuptureDamageMultiplier;
            UGameplayStatics::ApplyDamage(
                TargetPlayer,
                CalculatedDamage,
                GetController(),
                this,
                UDamageType::StaticClass()
            );
        }

        LastPlayerLocation = CurrentPlayerLocation;
    }
}

void ABloodseekerBoss::RotateTowardsPlayer(float DeltaTime)
{
    if (!TargetPlayer) return;

    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
        GetActorLocation(),
        TargetPlayer->GetActorLocation()
    );
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;

    FRotator CurrentRotation = GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaTime,
        RotationSpeed
    );
    SetActorRotation(NewRotation);
}

void ABloodseekerBoss::Attack()
{
    if (!BulletSpawner || !TargetPlayer) return;

    if (CurrentAttackState == EBossAttackState::Idle)
    {
        CurrentAttackState = EBossAttackState::LinearBurst;
    }

    switch (CurrentAttackState)
    {
    case EBossAttackState::LinearBurst:
        ExecuteLinearBurst();
        break;
    case EBossAttackState::RadialSphere:
        ExecuteRadialSphere();
        break;
    case EBossAttackState::UltimateMalediction:
        ExecuteUltimateMalediction();
        break;
    case EBossAttackState::Idle:
    default:
        break;
    }
}
void ABloodseekerBoss::ExecuteLinearBurst()
{
    if (!BulletSpawner) return;
    BulletSpawner->StartSequence(LinearBurstCombo);

    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        this,
        &ABloodseekerBoss::CycleNextAttack,
        TimeBetweenAttacks,
        false
    );
}

void ABloodseekerBoss::ExecuteRadialSphere()
{
    if (!BulletSpawner) return;
    BulletSpawner->StartSequence(RadialCombo);

    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        this,
        &ABloodseekerBoss::CycleNextAttack,
        TimeBetweenAttacks,
        false
    );
}

void ABloodseekerBoss::ExecuteUltimateMalediction()
{
    ActivateMalediction();

    if (BulletSpawner)
    {
        BulletSpawner->StartSequence(Combo2);
    }

    GetWorld()->GetTimerManager().SetTimer(
        RuptureTimerHandle,
        this,
        &ABloodseekerBoss::DeactivateMalediction,
        RuptureDuration,
        false
    );
}

void ABloodseekerBoss::CycleNextAttack()
{
    GetWorld()->GetTimerManager().ClearTimer(AttackCycleTimer);

  
    // Validación segura de porcentaje de salud si el HealthComponent existe
    float HealthPercent = 1.0f;
    if (HealthComp && HealthComp->MaxHealth > 0.0f)
    {
        HealthPercent = HealthComp->CurrentHealth / HealthComp->MaxHealth;
    }

    switch (CurrentAttackState)
    {
    case EBossAttackState::LinearBurst:
        CurrentAttackState = EBossAttackState::RadialSphere;
        break;

    case EBossAttackState::RadialSphere:
        // Prioridad: si la salud ≤ 50%, activamos la Ultimate
        if (HealthPercent <= 0.5f)
        {
            CurrentAttackState = EBossAttackState::UltimateMalediction;
        }
        else
        {
            CurrentAttackState = EBossAttackState::LinearBurst;
        }
        break;

    case EBossAttackState::UltimateMalediction:
        // Después de la Ultimate, volvemos al ciclo
        CurrentAttackState = EBossAttackState::LinearBurst;
        break;

    default:
        CurrentAttackState = EBossAttackState::LinearBurst;
        break;
    }
}

void ABloodseekerBoss::ActivateMalediction()
{
    bIsMaledictionActive = true;
    bIsRuptureActive = true;
    LastPlayerLocation = TargetPlayer->GetActorLocation();
}

void ABloodseekerBoss::DeactivateMalediction()
{
    bIsMaledictionActive = false;
    bIsRuptureActive = false;

    GetWorld()->GetTimerManager().ClearTimer(RuptureTimerHandle);

    // Reanudamos el ciclo de ataques normales
    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        this,
        &ABloodseekerBoss::CycleNextAttack,
        TimeBetweenAttacks,
        false
    );
}

void ABloodseekerBoss::Die()
{
    // Limpieza obligatoria de timers
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RuptureTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(AttackCycleTimer);
    }

    // Apagamos mecánicas activas
    bIsRuptureActive = false;
    bIsMaledictionActive = false;
    
    Super::Die();
}