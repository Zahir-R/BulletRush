#include "VaultKeeper/enemies/VaultKeeper.h"
#include "Components/WeakPointComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AVaultKeeper::AVaultKeeper()
{
    // WeakPoints: uno por cada parte vital
    // Se crean aquí para que BeginPlay() de BossBase los detecte automáticamente

    UWeakPointComponent* WP1 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Left"));
    WP1->SetupAttachment(RootComponent);
    WP1->SetRelativeLocation(FVector(0.0f, -200.0f, 0.0f));

    UWeakPointComponent* WP2 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Right"));
    WP2->SetupAttachment(RootComponent);
    WP2->SetRelativeLocation(FVector(0.0f, 200.0f, 0.0f));
    

    UWeakPointComponent* WP3 = CreateDefaultSubobject<UWeakPointComponent>(TEXT("WeakPoint_Core"));
    WP3->SetupAttachment(RootComponent);
    WP3->SetRelativeLocation(FVector(200.0f, 0.0f, 0.0f));

    // En AVaultKeeper::AVaultKeeper() — asigna materiales (reemplaza las rutas por las de tu proyecto)
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpenMatObj(TEXT("/Game/StarterContent/Materials/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse"));
    if (OpenMatObj.Succeeded())
    {
        OpenMaterial = OpenMatObj.Object;
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ClosedMatObj(TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile.M_Tech_Hex_Tile'"));
    if (ClosedMatObj.Succeeded())
    {
        ClosedMaterial = ClosedMatObj.Object;
    }
}

void AVaultKeeper::BeginPlay()
{
    Super::BeginPlay(); // BossBase ya cuenta los WeakPoints y suscribe HandleWeakPointDestroyed

    // Cacheamos los WeakPoints para no buscarlos cada frame
    GetComponents<UWeakPointComponent>(CachedWeakPoints);

    // Cacheamos al jugador
    CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

    // Empezamos cerrado, WeakPoints inactivos
    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP) WP->SetGenerateOverlapEvents(false);
    }
}

// ---------------------------------------------------------
// OVERRIDE DE ESTADO: en Idle arranca el ciclo cerrado
// ---------------------------------------------------------
void AVaultKeeper::SetBossState(EBossState NewState)
{
    Super::SetBossState(NewState);

    if (NewState == EBossState::Idle)
    {
        Close(); // El jefe empieza cerrado
    }
    else if (NewState == EBossState::Stunned)
    {
        // Detenemos el ciclo mientras está aturdido
        GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    }
    else if (NewState == EBossState::Attacking)
    {
        // Al volver de Stunned, reiniciamos el ciclo
        RegenerateWeakPoints();
        Close();
    }
}

// ---------------------------------------------------------
// OPEN: abre el jefe, activa WeakPoints, inicia curación
// ---------------------------------------------------------
void AVaultKeeper::Open()
{
    bIsOpen = true;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] Abierto — WeakPoints activos"));

    // Cambia material a estado Abierto (si están asignados)
    if (BossMesh && OpenMaterial)
    {
        BossMesh->SetMaterial(0, OpenMaterial);
    }

    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP && !WP->IsDestroyed()) {
            WP->SetGenerateOverlapEvents(true);
        }

    }

    // Curación pasiva cada segundo
    GetWorld()->GetTimerManager().SetTimer(
        HealTimer,
        this,
        &AVaultKeeper::ApplyPassiveHeal,
        1.0f,
        true // loop
    );

    // Después de OpenDuration, si sigue abierto cerramos
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer,
        [this]()
        {
            RegenerateWeakPoints();
            Close();
        },
        OpenDuration,
        false
    );
}

// ---------------------------------------------------------
// CLOSE: cierra el jefe, desactiva WeakPoints, ataca
// ---------------------------------------------------------
void AVaultKeeper::Close()
{
    bIsOpen = false;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] Cerrado — WeakPoints inactivos"));

    // Cambia material a estado Cerrado (si están asignados)
    if (BossMesh && ClosedMaterial)
    {
        BossMesh->SetMaterial(0, ClosedMaterial);
    }

    for (UWeakPointComponent* WP : CachedWeakPoints)
    {
        if (WP) WP->SetGenerateOverlapEvents(false);
    }

    // Detenemos curación
    GetWorld()->GetTimerManager().ClearTimer(HealTimer);

    // Patrón de ataque cerrado
    Attack();

    // Después de ClosedDuration abrimos
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer,
        this,
        &AVaultKeeper::Open,
        ClosedDuration,
        false
    );
}

// ---------------------------------------------------------
// CURACIÓN PASIVA
// ---------------------------------------------------------
void AVaultKeeper::ApplyPassiveHeal()
{
    if (HealthComp)
        HealthComp->Heal(HealRate);
}

// ---------------------------------------------------------
// REGENERAR WEAKPOINTS que no fueron destruidos
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
// ATAQUE: cerrado = Circle, abierto = guiado al jugador
// ---------------------------------------------------------
void AVaultKeeper::Attack()
{
    if (!BulletSpawner) return;

    if (!bIsOpen)
    {
        // Patrón cerrado: anillo expansivo, una bala por punto vital activo
        TArray<FAttackStep> PatronCerrado;
        int32 BulletCount = FMath::Max(ActiveWeakPoints * 8, 8);
        PatronCerrado.Add(FAttackStep(EAttackType::Circle, BulletCount, 400.0f, 0.5f));
        BulletSpawner->StartSequence(PatronCerrado);
    }
    else
    {
        // Patrón abierto: proyectiles guiados al jugador
        if (!CachedPlayer)
            CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

        if (!CachedPlayer) return;

        FVector Direction = (CachedPlayer->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector Origin = GetActorLocation() + Direction * 150.0f;

        TArray<FAttackStep> PatronAbierto;
        PatronAbierto.Add(FAttackStep(EAttackType::Burst, 5, 600.0f, 0.15f, Origin));
        BulletSpawner->StartSequence(PatronAbierto);
    }
}

// ---------------------------------------------------------
// WEAKPOINT DESTRUIDO
// ---------------------------------------------------------
void AVaultKeeper::HandleWeakPointDestroyed()
{
    ActiveWeakPoints--;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeper] WeakPoint destruido. Quedan: %d"), ActiveWeakPoints);

    if (ActiveWeakPoints <= 0)
    {
        // Todos destruidos: detener ciclo y aturdir
        GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
        GetWorld()->GetTimerManager().ClearTimer(HealTimer);
        SetBossState(EBossState::Stunned);
    }
    // Si quedan WeakPoints, el ciclo continúa normal
}

// ---------------------------------------------------------
// MUERTE
// ---------------------------------------------------------
void AVaultKeeper::Die()
{
    GetWorld()->GetTimerManager().ClearTimer(CycleTimer);
    GetWorld()->GetTimerManager().ClearTimer(HealTimer);
    GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
    Super::Die();
}