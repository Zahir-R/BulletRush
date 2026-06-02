
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Enemies/Bloodseeker/KamikazeEnemy.h"
#include "Enemies/State/BossStateBase.h"
#include "Enemies/State/BossStateAttacking.h"
#include "Player/PlayingPlayer.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Core/Bloodseeker/BloodseekerGameMode.h"
#include "Core/Bloodseeker/BloodseekerFacade.h"

ABloodseekerBoss::ABloodseekerBoss()
{
    PrimaryActorTick.bCanEverTick = true;

    // Movimiento sinusoidal
    MovementAmplitude = 350.0f;
    MovementFrequency = 1.5f;

    
    RotationSpeed = 10.0f;

    // Control de fases de ataque
    CurrentAttackState = EBossAttackState::Idle;
    TimeBetweenAttacks = 3.0f;

    // Rupture / Malediction
    bIsRuptureActive = false;
    bIsMaledictionActive = false;
    RuptureDamageMultiplier = 0.03f;
    RuptureDuration = 30.0f;
    LastPlayerLocation = FVector::ZeroVector;

    // Ultimate por tiempo
    UltimateCount = 0;

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

	//Ataque en abanico 120 grados
    LinearBurstCombo.Add(FAttackStep(EAttackType::Fan, 50, 500.0f, 0.1f, 100.0f));

    // Explosión radial 360 grados
    RadialCombo.Add(FAttackStep(EAttackType::Sphere, 1000, 800.0f, 0.5f, 0.1f));

    if (HealthComp)
    {
        HealthComp->CurrentHealth = HealthComp->MaxHealth;
        HealthComp->OnHealthChanged.AddDynamic(this, &ABloodseekerBoss::OnBossHealthChanged);
    }

    // Auto-start attack cycle after 3s, bypassing weak point destruction requirement
    TWeakObjectPtr<ABloodseekerBoss> WeakBoss(this);
    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        [WeakBoss]()
        {
            ABloodseekerBoss* Self = WeakBoss.Get();
            if (!Self || !Self->GetWorld()) return;
            if (Self->GetCurrentBossStateName() == "Idle" || Self->GetCurrentBossStateName() == "Intro")
            {
                Self->ChangeState(Self->AttackingState);
            }
        },
        3.0f,
        false
    );

    // Timer de Ultimate cada 20 segundos
    GetWorld()->GetTimerManager().SetTimer(
        UltimateTimerHandle,
        this,
        &ABloodseekerBoss::TryActivateUltimate,
        20.0f,
        true
    );
}

void ABloodseekerBoss::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetPlayer || GetCurrentBossStateName() == "Dead") return;

    // ------- Rotacion suave hacia el jugador
    RotateTowardsPlayer(DeltaTime);

    // ------- Movimiento sinusoidal (si no esta stuneado ni en Ultimate)
    if (!bIsMaledictionActive && GetCurrentBossStateName() != "Stunned")
    {
        FVector NewLocation = InitialLocation;
        float TimeSecs = GetWorld()->GetTimeSeconds();
        NewLocation += GetActorRightVector()
            * FMath::Sin(TimeSecs * MovementFrequency)
            * MovementAmplitude;
        SetActorLocation(NewLocation);
    }

    // ------- Calculo de sangrado Rupture 3 ejes
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

    // ------- Láser rojo de debug (Malediction) -------
    if (bIsMaledictionActive && TargetPlayer)
    {
        DrawDebugLine(
            GetWorld(),
            GetActorLocation(),
            TargetPlayer->GetActorLocation(),
            FColor::Red,
            false,
            -1.0f,
            0,
            3.0f
        );
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

void ABloodseekerBoss::TryActivateUltimate()
{
    if (!IsValid(this) || !GetWorld()) return;
    if (bIsMaledictionActive || bIsRuptureActive) return;
    if (!HealthComp || HealthComp->CurrentHealth <= 0.0f) return;

    SetInvulnerable(true);

    UltimateCount = 0;
    bIsMaledictionActive = true;
    bIsRuptureActive = true;

    if (TargetPlayer)
    {
        LastPlayerLocation = TargetPlayer->GetActorLocation();
    }

    if (BulletSpawner)
    {
        BulletSpawner->StopCurrentSequence();
    }

    if (FacadeRef)
    {
        FacadeRef->PauseBossWaves();
    }

    GetWorld()->GetTimerManager().SetTimer(
        RuptureTimerHandle,
        this,
        &ABloodseekerBoss::DeactivateMalediction,
        RuptureDuration,
        false
    );

    SpawnKamikazeWave(3, 0.3f);

    TWeakObjectPtr<ABloodseekerBoss> WeakBoss_KW1(this);
    GetWorld()->GetTimerManager().SetTimer(
        KamikazeWaveTimer,
        [WeakBoss_KW1]()
        {
            ABloodseekerBoss* Self = WeakBoss_KW1.Get();
            if (!Self || !Self->GetWorld() || !Self->bIsMaledictionActive) return;
            Self->SpawnKamikazeWave(5, 0.25f);
        },
        5.0f,
        false
    );

    TWeakObjectPtr<ABloodseekerBoss> WeakBoss_KW2(this);
    GetWorld()->GetTimerManager().SetTimer(
        KamikazeWaveTimer2,
        [WeakBoss_KW2]()
        {
            ABloodseekerBoss* Self = WeakBoss_KW2.Get();
            if (!Self || !Self->GetWorld() || !Self->bIsMaledictionActive) return;
            Self->SpawnKamikazeWave(8, 0.2f);
        },
        10.0f,
        false
    );
}

void ABloodseekerBoss::ExecuteUltimateMalediction()
{
    TryActivateUltimate();
}

void ABloodseekerBoss::CycleNextAttack()
{
    AttackCycleIndex = 1 - AttackCycleIndex;

    AttackSequence = (AttackCycleIndex == 0) ? LinearBurstCombo : RadialCombo;

    if (BulletSpawner)
    {
        BulletSpawner->StartSequence(AttackSequence);
    }

    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        this,
        &ABloodseekerBoss::CycleNextAttack,
        TimeBetweenAttacks,
        false
    );
}

void ABloodseekerBoss::ActivateMalediction()
{
    bIsMaledictionActive = true;
    bIsRuptureActive = true;
    if (TargetPlayer)
    {
        LastPlayerLocation = TargetPlayer->GetActorLocation();
    }
}

void ABloodseekerBoss::DeactivateMalediction()
{
    bIsMaledictionActive = false;
    bIsRuptureActive = false;

    // Boss vuelve a recibir daño
    SetInvulnerable(false);

    // Limpiar timers de oleadas
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RuptureTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle2);
        GetWorld()->GetTimerManager().ClearTimer(KamikazeWaveTimer);
        GetWorld()->GetTimerManager().ClearTimer(KamikazeWaveTimer2);
    }

    if (FacadeRef)
    {
        FacadeRef->ResumeBossWaves();
    }

    // Reanudamos el ciclo de ataques normales
    GetWorld()->GetTimerManager().SetTimer(
        AttackCycleTimer,
        this,
        &ABloodseekerBoss::CycleNextAttack,
        TimeBetweenAttacks,
        false
    );
}

void ABloodseekerBoss::SpawnKamikazeWave(int32 Count, float DelayBetween)
{
    if (!TargetPlayer || !GetWorld()) return;

    FVector PlayerLocation = TargetPlayer->GetActorLocation();
    FVector PlayerForward = TargetPlayer->GetActorForwardVector();
    PlayerForward.Z = 0.0f;
    if (PlayerForward.IsNearlyZero()) PlayerForward = FVector::ForwardVector;
    PlayerForward.Normalize();

    for (int32 i = 0; i < Count; i++)
    {
        float SpawnDelay = DelayBetween * i;
        if (SpawnDelay <= 0.0f) SpawnDelay = 0.01f;

        FTimerHandle SpawnHandle;
        TWeakObjectPtr<ABloodseekerBoss> WeakBoss_SW(this);
        GetWorld()->GetTimerManager().SetTimer(
            SpawnHandle,
            [WeakBoss_SW, PlayerLocation, PlayerForward]()
            {
                ABloodseekerBoss* Self = WeakBoss_SW.Get();
                if (!Self || !Self->GetWorld()) return;

                // Ángulo horizontal: +-120grados desde el frente del player (excluye espalda)
                float HorzAngle = FMath::FRandRange(-120.0f, 120.0f);
                FVector HorzDir = PlayerForward.RotateAngleAxis(HorzAngle, FVector::UpVector);

                // Distancia: 1800-2500 unidades
                float Distance = FMath::FRandRange(1800.0f, 2500.0f);

                // Altura: +-300 unidades
                float HeightOffset = FMath::FRandRange(-300.0f, 300.0f);

                FVector SpawnLocation = PlayerLocation + HorzDir * Distance;
                SpawnLocation.Z += HeightOffset;

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                AKamikazeEnemy* NewKamikaze = Self->GetWorld()->SpawnActor<AKamikazeEnemy>(
                    AKamikazeEnemy::StaticClass(),
                    FTransform(SpawnLocation),
                    SpawnParams
                );

                if (NewKamikaze)
                {
                    NewKamikaze->InitializeArc(SpawnLocation, PlayerLocation);
                }
            },
            SpawnDelay,
            false
        );
    }
}

void ABloodseekerBoss::OnBossHealthChanged(float NewHealth)
{
    if (HealthComp && HealthComp->MaxHealth > 0.0f)
    {
        float HealthPercent = HealthComp->CurrentHealth / HealthComp->MaxHealth;
        if (HealthPercent <= 0.0f && !bIsMaledictionActive)
        {
            UltimateCount = 0;
        }
    }
}

void ABloodseekerBoss::Die()
{
    // Limpieza obligatoria de timers
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(RuptureTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(AttackCycleTimer);
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle2);
        GetWorld()->GetTimerManager().ClearTimer(UltimateTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(KamikazeWaveTimer);
        GetWorld()->GetTimerManager().ClearTimer(KamikazeWaveTimer2);
    }

    // Apagamos mecanicas activas
    bIsRuptureActive = false;
    bIsMaledictionActive = false;
    
    Super::Die();
}