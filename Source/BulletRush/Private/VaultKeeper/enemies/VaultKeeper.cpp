#include "VaultKeeper/enemies/VaultKeeper.h"
#include "Components/WeakPointComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Enemies/State/BossStateBase.h"
#include "Enemies/State/BossStateStunned.h"
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
    Super::BeginPlay(); // BossBase crea los estados y llama ChangeState(IntroState)

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
        float TargetZ = HomeZ + FMath::Sin(GetWorld()->GetTimeSeconds() * HoverFrequency) * HoverAmplitude;
        CurrentZ = FMath::FInterpTo(CurrentZ, TargetZ, DeltaTime, 3.0f);

        FRotator NewRot = GetActorRotation();
        NewRot.Yaw += RotationRate * DeltaTime;
        SetActorRotation(NewRot);
    }
    else
    {
        float StunnedZ = HomeZ - 50.0f;
        CurrentZ = FMath::FInterpTo(CurrentZ, StunnedZ, DeltaTime, 2.0f);
    }

    SetActorLocation(FVector(Loc.X, Loc.Y, CurrentZ));
}

void AVaultKeeper::ChangeState(UBossState* NewState)
{
    Super::ChangeState(NewState);

    if (!NewState) return;

    FName StateName = NewState->GetStateTagName();

    if (StateName == "Idle")
    {
        Close();
    }
    else if (StateName == "Attacking")
    {
        bIsStunned = false;
        RegenerateWeakPoints();
        Close();
    }
    else if (StateName == "Stunned")
    {
        bIsStunned = true;
        ClearAllTimers();
    }
    else if (StateName == "PhaseTransition")
    {
        // Fase critica: mas agresivo
        ClosedDuration = 2.0f;
        OpenDuration = 3.0f;
        ClearAllTimers();
    }
    else if (StateName == "Dead")
    {
        bIsStunned = true;
        ClearAllTimers();
    }
}

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

    // Curacion pasiva
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

    // Ataque inmediato
    Attack();

    // Loop de ataque cerrado
    GetWorld()->GetTimerManager().SetTimer(
        AttackLoopTimer, this, &AVaultKeeper::Attack, 2.5f, true);

    // Timer para abrir
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer, this, &AVaultKeeper::Open, ClosedDuration, false);
}

void AVaultKeeper::ClearAllTimers()
{
    GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
}

void AVaultKeeper::Attack()
{
    if (!BulletSpawner) return;

    if (!bIsOpen)
    {
        switch (AttackIdentifier % 3)
        {
        case 0:
        {
            TArray<FAttackStep> VK_Closed0;
            VK_Closed0.Add(FAttackStep(EAttackType::Circle, 24, 400.0f, 0.3f));
            VK_Closed0.Add(FAttackStep(EAttackType::Spiral, 16, 350.0f, 0.5f, 15.0f));
            VK_Closed0.Add(FAttackStep(EAttackType::Spiral, 16, 350.0f, 0.5f, -15.0f));
            BulletSpawner->StartSequence(VK_Closed0);
            break;
        }
        case 1:
        {
            if (!CachedPlayer)
                CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
            if (!CachedPlayer) break;

            FVector Dir = (CachedPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FVector Origin = GetActorLocation() + Dir * 150.0f;

            TArray<FAttackStep> VK_Closed1;
            VK_Closed1.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            VK_Closed1.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            VK_Closed1.Add(FAttackStep(EAttackType::Burst, 6, 700.0f, 0.1f, Origin));
            BulletSpawner->StartSequence(VK_Closed1);
            break;
        }
        case 2:
        {
            TArray<FAttackStep> VK_Closed2;
            VK_Closed2.Add(FAttackStep(EAttackType::Sphere, 200, 250.0f, 0.4f, 0.1f));
            VK_Closed2.Add(FAttackStep(EAttackType::Circle, 20, 600.0f, 0.3f));
            BulletSpawner->StartSequence(VK_Closed2);
            break;
        }
        }
        AttackIdentifier++;
    }
    else
    {
        if (!CachedPlayer)
            CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
        if (!CachedPlayer) return;

        FVector Dir = (CachedPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        switch (OpenAttackIndex % 2)
        {
        case 0:
        {
            FVector Origin = GetActorLocation() + Dir * 150.0f;
            TArray<FAttackStep> VK_Open0;
            VK_Open0.Add(FAttackStep(EAttackType::Burst, 8, 650.0f, 0.12f, Origin));
            BulletSpawner->StartSequence(VK_Open0);
            break;
        }
        case 1:
        {
            FVector PlayerPos = CachedPlayer->GetActorLocation();
            TArray<FAttackStep> VK_Open1;
            VK_Open1.Add(FAttackStep(EAttackType::Spiral, 20, 400.0f, 0.3f, PlayerPos, 12.0f));
            BulletSpawner->StartSequence(VK_Open1);
            break;
        }
        }
        OpenAttackIndex++;
    }
}

void AVaultKeeper::HandleWeakPointDestroyed()
{
    ActiveWeakPoints--;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] WeakPoint destruido. Quedan: %d"), ActiveWeakPoints);

    RageAttack();

    if (ActiveWeakPoints <= 0)
    {
        ClearAllTimers();
        ChangeState(StunnedState); // Usa el objeto de estado del BossBase
    }
}

void AVaultKeeper::RageAttack()
{
    if (!BulletSpawner) return;
    TArray<FAttackStep> VK_Rage;
    VK_Rage.Add(FAttackStep(EAttackType::Sphere, 200, 500.0f, 0.2f, 0.1f));
    BulletSpawner->StartSequence(VK_Rage);
}

void AVaultKeeper::ApplyPassiveHeal()
{
    if (HealthComp) HealthComp->Heal(HealRate);
}

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

void AVaultKeeper::UpdateWeakPointMaterials(bool bOpen)
{
    UMaterialInterface* Mat = bOpen ? WPOpenMaterial : WPClosedMaterial;
    for (UWeakPointComponent* WP : CachedWeakPoints)
        if (WP && !WP->IsDestroyed())
            WP->SetVisualMaterial(Mat);
}

void AVaultKeeper::Die()
{
    bIsStunned = true;
    ClearAllTimers();
    Super::Die(); // BossBase llama ChangeState(DeadState)
}
void AVaultKeeper::DestroyOneWeakPoint()
{
    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP && !WP->IsDestroyed())
        {
            WP->ForceDestroy();
            UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] WP destruido por recompensa 2-S"));
            return;
        }
    }
}