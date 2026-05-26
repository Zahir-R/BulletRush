#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Chronostasis/GameModeChronostasis.h"
#include "Core/Chronostasis/ChronostasisEnemyFactory.h"
#include "Core/BulletRushGameModeBase.h"
#include "Enemies/EnemyBase.h"
#include "Components/BuffComponent.h"
#include "Buffs/TimeSlowDecorator.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"

AChronostasisFacade::AChronostasisFacade()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentWaveIndex = 0;
    RemainingEnemiesInWave = 0;
    SlowTriggerCount = 0;
}

AChronostasisFacade::~AChronostasisFacade()
{
}

void AChronostasisFacade::BeginPlay()
{
    Super::BeginPlay();
    Factory = MakeUnique<FChronostasisEnemyFactory>();

    // Waves hardcodeadas waos
    if (Waves.Num() == 0)
    {
        FWaveConfig W1; W1.DroneCount = 3; W1.SpawnPoints = { FVector(100,0,0), FVector(-100,0,0), FVector(0,100,0) };
        FWaveConfig W2; W2.DroneCount = 2; W2.MassCount = 1; W2.SpawnPoints = { FVector(200,0,0), FVector(-200,0,0), FVector(0,200,0) };
        FWaveConfig W3; W3.DroneCount = 1; W3.MassCount = 1; W3.ExpansiveCount = 1; W3.SpawnPoints = { FVector(300,0,0), FVector(-300,0,0), FVector(0,300,0) };
        Waves = { W1, W2, W3 };
    }

    OwningGameMode = Cast<ABulletRushGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AChronostasisFacade::StartGame()
{
    CurrentWaveIndex = 0;
    SlowTriggerCount = 0;
    bPlayerTookDamage = false;
    // Observe player health changes (Observer pattern)
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UHealthComponent* HealthComp = PlayerPawn->FindComponentByClass<UHealthComponent>();
        if (HealthComp)
        {
            PlayerHealthComp = HealthComp;
            HealthComp->OnHealthChanged.AddDynamic(this, &AChronostasisFacade::OnPlayerHealthChanged);
        }
    }
    StartSlowTimer();
    StartWave(0);
}

void AChronostasisFacade::StartSlowTimer()
{
    GetWorldTimerManager().SetTimer(SlowTimerHandle, this, &AChronostasisFacade::OnSlowTimerExpired, 15.0f, false);
}

void AChronostasisFacade::OnSlowTimerExpired()
{
    SlowTriggerCount++;
    UE_LOG(LogTemp, Warning, TEXT("AChronostasisFacade::OnSlowTimerExpired: Incrementado SlowTriggerCount a %d"), SlowTriggerCount);
    
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            // TimeSlowDecorator no acumulativo
            if (!BuffComp->HasDecoratorOfClass(UTimeSlowDecorator::StaticClass()))
            {
                UE_LOG(LogTemp, Warning, TEXT("Aplicando TimeSlowDecorator al jugador"));
                BuffComp->ApplyBuff(UTimeSlowDecorator::StaticClass(), -1.0f, 0.0f);
            }
        }
    }
    StartSlowTimer();
    // Notify observers that a time stop happened
    OnTimeStop.Broadcast();
}

void AChronostasisFacade::StartWave(int32 Index)
{
    UE_LOG(LogTemp, Warning, TEXT("Intentando iniciar oleada %d"), Index);
    if (!Factory)
    {
        UE_LOG(LogTemp, Error, TEXT("Factory es nulo"));
        return;
    }
    if (!Waves.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("Indice de oleada %d invalido. Total oleadas: %d"), Index, Waves.Num());
        return;
    }

    CurrentWaveIndex = Index;
    const FWaveConfig& Cfg = Waves[Index];
    RemainingEnemiesInWave = Cfg.DroneCount + Cfg.MassCount + Cfg.ExpansiveCount;
    UE_LOG(LogTemp, Warning, TEXT("Iniciando oleada %d con Drones: %d, Mass: %d, Expansive: %d"), 
        Index, Cfg.DroneCount, Cfg.MassCount, Cfg.ExpansiveCount);

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World es nulo"));
        return;
    }
    int32 SpawnIndex = 0;
    for (int32 i=0;i<Cfg.DroneCount;i++)
    {
        FVector Loc = /*Cfg.SpawnPoints.IsValidIndex(SpawnIndex) ? */ Cfg.SpawnPoints[SpawnIndex] /*: GetActorLocation()*/;
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Drone en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = Factory->CreateDrone(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Drone spawneado con exito"));
            // Suscribir a EnemyDeath
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Drone"));
        }
    }
    for (int32 i=0;i<Cfg.MassCount;i++)
    {
        FVector Loc = /*Cfg.SpawnPoints.IsValidIndex(SpawnIndex) ?*/ Cfg.SpawnPoints[SpawnIndex] /*: GetActorLocation()*/;
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Mass en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = Factory->CreateMass(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Mass spawneado con exito"));
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Mass"));
        }
    }
    for (int32 i=0;i<Cfg.ExpansiveCount;i++)
    {
        FVector Loc = /*Cfg.SpawnPoints.IsValidIndex(SpawnIndex) ?*/ Cfg.SpawnPoints[SpawnIndex] /*: GetActorLocation()*/;
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Expansive en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = Factory->CreateExpansive(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Expansive spawneado con exito"));
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Expansive"));
        }
    }
}

void AChronostasisFacade::OnEnemyKilled(AEnemyBase* Enemy)
{
    RemainingEnemiesInWave = FMath::Max(0, RemainingEnemiesInWave - 1);
    // Reset SlowTimer y eliminar decorador
    GetWorldTimerManager().ClearTimer(SlowTimerHandle);
    StartSlowTimer();
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            BuffComp->RemoveDecoratorByClass(UTimeSlowDecorator::StaticClass());
        }
    }

    if (RemainingEnemiesInWave == 0)
    {
        if (CurrentWaveIndex + 1 < Waves.Num())
        {
            StartWave(CurrentWaveIndex + 1);
        }
        else
        {
            OnAllWavesComplete();
        }
    }
}

void AChronostasisFacade::OnAllWavesComplete()
{
    if (OwningGameMode)
    {
        bool bSecretUnlocked = (SlowTriggerCount >= 3) && !bPlayerTookDamage;
        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM)
        {
            if (bSecretUnlocked) GM->ActivateSecretPortal();
            else GM->ActivateBossPortal();
        }
    }
    GetWorldTimerManager().ClearTimer(SlowTimerHandle);
}

void AChronostasisFacade::OnPlayerHealthChanged(float NewHealth)
{
    if (!PlayerHealthComp.IsValid()) return;
    // If health decreased below max, consider player took damage
    if (NewHealth < PlayerHealthComp->MaxHealth)
    {
        bPlayerTookDamage = true;
        // Optionally unbind to avoid further calls
        PlayerHealthComp->OnHealthChanged.RemoveDynamic(this, &AChronostasisFacade::OnPlayerHealthChanged);
    }
}

void AChronostasisFacade::ActivatePortalToSecret()
{
    if (OwningGameMode)
    {
        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM) GM->ActivateSecretPortal();
    }
}

void AChronostasisFacade::ActivatePortalToBoss()
{
    if (OwningGameMode)
    {
        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM) GM->ActivateBossPortal();
    }
}
