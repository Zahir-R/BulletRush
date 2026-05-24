#include "VaultKeeper/enemies/VaultKeeper.h"
#include "Components/WeakPointComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AVaultKeeper::AVaultKeeper()
{
    PrimaryActorTick.bCanEverTick = true;

    UWeakPointComponent* WP1 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Left"));
    WP1->SetupAttachment(RootComponent);
    WP1->SetRelativeLocation(FVector(0.0f, -200.0f, 0.0f));

    UWeakPointComponent* WP2 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Right"));
    WP2->SetupAttachment(RootComponent);
    WP2->SetRelativeLocation(FVector(0.0f, 200.0f, 0.0f));

    UWeakPointComponent* WP3 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Core"));
    WP3->SetupAttachment(RootComponent);
    WP3->SetRelativeLocation(FVector(200.0f, 0.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpenMatObj(
        TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse'"));
    if (OpenMatObj.Succeeded()) OpenMaterial = OpenMatObj.Object;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ClosedMatObj(
        TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile.M_Tech_Hex_Tile'"));
    if (ClosedMatObj.Succeeded()) ClosedMaterial = ClosedMatObj.Object;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> WPOpenMatObj(
        TEXT("Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
    if (WPOpenMatObj.Succeeded()) WPOpenMaterial = WPOpenMatObj.Object;

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> WPClosedMatObj(
        TEXT("Material'/Game/StarterContent/Materials/M_Metal_Steel.M_Metal_Steel'"));
    if (WPClosedMatObj.Succeeded()) WPClosedMaterial = WPClosedMatObj.Object;
}

void AVaultKeeper::BeginPlay()
{
    Super::BeginPlay();

    GetComponents<UWeakPointComponent>(CachedWeakPoints);
    CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

    HomeZ = GetActorLocation().Z;
    CurrentZ = HomeZ;

    for (UWeakPointComponent* WP : CachedWeakPoints)
        if (WP) WP->SetGenerateOverlapEvents(false);
}

void AVaultKeeper::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector Loc = GetActorLocation();

    if (!bIsStunned)
    {
        // Flotacion
        float TargetZ = HomeZ + FMath::Sin(GetWorld()->GetTimeSeconds() * HoverFrequency) * HoverAmplitude;
        CurrentZ = FMath::FInterpTo(CurrentZ, TargetZ, DeltaTime, 3.0f);

        // Rotacion constante sobre Z
        FRotator NewRot = GetActorRotation();
        NewRot.Yaw += RotationRate * DeltaTime;
        SetActorRotation(NewRot);
    }
    else
    {
        // Stunned: cae levemente
        float StunnedZ = HomeZ - 50.0f;
        CurrentZ = FMath::FInterpTo(CurrentZ, StunnedZ, DeltaTime, 2.0f);
    }

    SetActorLocation(FVector(Loc.X, Loc.Y, CurrentZ));
}

// ---------------------------------------------------------
// ESTADO
// ---------------------------------------------------------
void AVaultKeeper::SetBossState(EBossState NewState)
{
    Super::SetBossState(NewState);

    if (NewState == EBossState::Idle)
    {
        Close();
    }
    else if (NewState == EBossState::Stunned)
    {
        bIsStunned = true;
        GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
        GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
    }
    else if (NewState == EBossState::Attacking)
    {
        bIsStunned = false;
        RegenerateWeakPoints();
        Close();
    }
    else if (NewState == EBossState::PhaseTransition)
    {
        // Fase critica: más agresivo
        ClosedDuration = 2.0f;
        OpenDuration = 3.0f;
    }
}

// ---------------------------------------------------------
// OPEN
// ---------------------------------------------------------
void AVaultKeeper::Open()
{
    bIsOpen = true;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] Abierto"));

    if (MeshEnemy && OpenMaterial)
        MeshEnemy->SetMaterial(0, OpenMaterial);

    UpdateWeakPointMaterials(true);

    for (UWeakPointComponent* WP : CachedWeakPoints)
        if (WP && !WP->IsDestroyed())
            WP->SetGenerateOverlapEvents(true);

    // Curación pasiva
    GetWorld()->GetTimerManager().SetTimer(
        HealTimer, this, &AVaultKeeper::ApplyPassiveHeal, 1.0f, true);

    // Loop de ataque abierto
    GetWorld()->GetTimerManager().SetTimer(
        AttackLoopTimer, this, &AVaultKeeper::Attack, 1.8f, true);

    // Timer para cerrar
    GetWorld()->GetTimerManager().SetTimer(CycleTimer, [this]()
        {
            RegenerateWeakPoints();
            Close();
        }, OpenDuration, false);
}

// ---------------------------------------------------------
// CLOSE
// ---------------------------------------------------------
void AVaultKeeper::Close()
{
    bIsOpen = false;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] Cerrado"));

    if (MeshEnemy && ClosedMaterial)
        MeshEnemy->SetMaterial(0, ClosedMaterial);

    UpdateWeakPointMaterials(false);

    for (UWeakPointComponent* WP : CachedWeakPoints)
        if (WP) WP->SetGenerateOverlapEvents(false);

    GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);

    // Ataque inmediato al cerrarse
    Attack();

    // Loop de ataque cerrado
    GetWorld()->GetTimerManager().SetTimer(
        AttackLoopTimer, this, &AVaultKeeper::Attack, 2.5f, true);

    // Timer para abrir
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer, this, &AVaultKeeper::Open, ClosedDuration, false);
}

// ---------------------------------------------------------
// ATAQUE
// ---------------------------------------------------------
void AVaultKeeper::Attack()
{
    if (!BulletSpawner) return;

    if (!bIsOpen)
    {
        // 3 patrones cerrado rotando con AttackIdentifier
        switch (AttackIdentifier % 3)
        {
        case 0:
        {
            // Circulo + espiral doble
            TArray<FAttackStep> Combo1;
            Combo1.Add(FAttackStep(EAttackType::Circle, 24, 400.0f, 0.3f));
            Combo1.Add(FAttackStep(EAttackType::Spiral, 16, 350.0f, 0.5f, 15.0f));
            Combo1.Add(FAttackStep(EAttackType::Spiral, 16, 350.0f, 0.5f, -15.0f));
            BulletSpawner->StartSequence(Combo1);
            break;
        }
        case 1:
        {
            // 3 bursts rapidos hacia el jugador
            if (!CachedPlayer)
                CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
            if (!CachedPlayer) break;

            FVector Dir = (CachedPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FVector Origin = GetActorLocation() + Dir * 150.0f;

            TArray<FAttackStep> Combo21;
            Combo21.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            Combo21.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            Combo21.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            BulletSpawner->StartSequence(Combo21);
            break;
        }
        case 2:
        {
            // Esfera lenta + circulo rapido encima
            TArray<FAttackStep> Combo3;
            Combo3.Add(FAttackStep(EAttackType::Sphere, 200, 250.0f, 0.4f, 0.1f));
            Combo3.Add(FAttackStep(EAttackType::Circle, 20, 600.0f, 0.3f));
            BulletSpawner->StartSequence(Combo3);
            break;
        }
        }
        AttackIdentifier++;
    }
    else
    {
        // 2 patrones abierto alternando
        if (!CachedPlayer)
            CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
        if (!CachedPlayer) return;

        FVector Dir = (CachedPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        switch (OpenAttackIndex % 2)
        {
        case 0:
        {
            // Burst guiado rapido
            FVector Origin = GetActorLocation() + Dir * 150.0f;
            TArray<FAttackStep> Combo4;
            Combo4.Add(FAttackStep(EAttackType::Burst, 8, 650.0f, 0.12f, Origin));
            BulletSpawner->StartSequence(Combo4);
            break;
        }
        case 1:
        {
            // Espiral desde posicion del jugador
            //FVector PlayerPos = CachedPlayer->GetActorLocation();
            //TArray<FAttackStep> Combo5;
            //Combo5.Add(FAttackStep(EAttackType::Spiral, 20, 400.0f, 0.3f, PlayerPos, 12.0f));
            //BulletSpawner->StartSequence(Combo5);
            break;
        }
        }
        OpenAttackIndex++;
    }
}

// ---------------------------------------------------------
// WEAKPOINT DESTRUIDO
// ---------------------------------------------------------
void AVaultKeeper::HandleWeakPointDestroyed()
{
    ActiveWeakPoints--;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] WeakPoint destruido. Quedan: %d"), ActiveWeakPoints);

    // Ataque de rabia inmediato
    RageAttack();

    if (ActiveWeakPoints <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
        GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
        SetBossState(EBossState::Stunned);
    }
}

void AVaultKeeper::RageAttack()
{
    if (!BulletSpawner) return;
    TArray<FAttackStep> Rage;
    Rage.Add(FAttackStep(EAttackType::Sphere, 200, 500.0f, 0.2f, 0.1f));
    BulletSpawner->StartSequence(Rage);
}

// ---------------------------------------------------------
// CURACIÓN PASIVA
// ---------------------------------------------------------
void AVaultKeeper::ApplyPassiveHeal()
{
    if (HealthComp) HealthComp->Heal(HealRate);
}

// ---------------------------------------------------------
// REGENERAR WEAKPOINTS
// ---------------------------------------------------------
void AVaultKeeper::RegenerateWeakPoints()
{
    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP && WP->IsDestroyed())
        {
            WP->ResetWeakPoint();
            ActiveWeakPoints++;
            UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] WeakPoint regenerado: %s"), *WP->GetName());
        }
    }
}

// ---------------------------------------------------------
// MATERIALES WEAKPOINTS
// ---------------------------------------------------------
void AVaultKeeper::UpdateWeakPointMaterials(bool bOpen)
{
    UMaterialInterface* Mat = bOpen ? WPOpenMaterial : WPClosedMaterial;
    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP && !WP->IsDestroyed())
            WP->SetVisualMaterial(Mat);
    }
}

// ---------------------------------------------------------
// MUERTE
// ---------------------------------------------------------
void AVaultKeeper::Die()
{
    bIsStunned = true;
    GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
    Super::Die();
}