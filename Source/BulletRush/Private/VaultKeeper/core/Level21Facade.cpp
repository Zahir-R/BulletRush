#include "VaultKeeper/core/Level21Facade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "VaultKeeper/enemies/MechaKamikazeEnemy.h"
#include "VaultKeeper/enemies/MechaChargerEnemy.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameInstance.h"
#include "Components/HealthComponent.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALevel21Facade::ALevel21Facade()
{
    PrimaryActorTick.bCanEverTick = false;

   
    // Spawn locations por defecto si no se asignan en el editor
    DroneSpawnLocations = {
        FVector(500.f,    0.f, 100.f),
        FVector(-500.f,   0.f, 100.f),
        FVector(0.f,    500.f, 100.f),
        FVector(0.f,   -500.f, 100.f),
        FVector(700.f,  300.f, 100.f),
        FVector(-700.f, 300.f, 100.f),
        FVector(300.f,  700.f, 100.f),
        FVector(-300.f,-700.f, 100.f),
        FVector(900.f,    0.f, 100.f),
        FVector(-900.f,   0.f, 100.f),
        FVector(0.f,    900.f, 100.f),
        FVector(0.f,   -900.f, 100.f),
        FVector(600.f, -600.f, 100.f),
        FVector(-600.f, 600.f, 100.f),
        FVector(1000.f, 400.f, 100.f),
    };
}

void ALevel21Facade::BeginPlay()
{
    Super::BeginPlay();

    // Spawneamos el factory
    Factory = GetWorld()->SpawnActor<AMechaEnemyFactory>(
        AMechaEnemyFactory::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator
    );
    PortalToBoss = GetWorld()->SpawnActor<ALevelPortal>(
        ALevelPortal::StaticClass(),
        FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
    PortalToBoss->SetActorHiddenInGame(true);
}


void ALevel21Facade::StartLevel()
{
    // Suscribimos la muerte del jugador
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    

    // Timer de 3 minutos
    GetWorld()->GetTimerManager().SetTimer(
        LevelTimer,
        this,
        &ALevel21Facade::OnTimerExpired,
        LevelTimeLimit,
        false
    );

    SpawnWave();
}


void ALevel21Facade::SpawnWave()
{
    if (!Factory) return;

    // 8 Drones
    
    TArray<FVector> DronePositions = {
        FVector(500.f,   0.f, 100.f), FVector(-500.f,  0.f, 100.f),
        FVector(0.f,   500.f, 100.f), FVector(0.f,  -500.f, 100.f),
        FVector(700.f, 300.f, 100.f), FVector(-700.f, 300.f, 100.f),
        FVector(300.f, 700.f, 100.f), FVector(-300.f,-700.f, 100.f),
    };

    // 4 Chargers
    TArray<FVector> ChargerPositions = {
        FVector(1900.f,   0.f, 300.f), FVector(-1900.f,   0.f, 300.f),
        FVector(0.f,   1900.f, 100.f), FVector(0.f,   -1900.f, 100.f),
    };

    // 3 Kamikazes
    TArray<FVector> KamikazePositions = {
        FVector(1600.f, -600.f, 100.f),
        FVector(-600.f, 600.f, 1100.f),
        FVector(1000.f, 1400.f, 100.f),
    };

    // Spawn drones
    
    for (FVector Loc : DronePositions)
    {
        ADronMecha* Drone = Cast<ADronMecha>(
            Factory->CreateEnemy(EMechaEnemyType::DroneMecha, Loc, FRotator::ZeroRotator));
        if (!Drone) continue;
        Drone->OnEnemyDeath.AddDynamic(this, &ALevel21Facade::OnEnemyKilled);
        ActiveEnemies.Add(Drone);
    }

    // Spawn chargers
    for (FVector Loc : ChargerPositions)
    {
        AMechaChargerEnemy* Charger = Cast<AMechaChargerEnemy>(
            Factory->CreateEnemy(EMechaEnemyType::MechaCharger, Loc, FRotator::ZeroRotator));
        if (!Charger) continue;
        Charger->OnEnemyDeath.AddDynamic(this, &ALevel21Facade::OnEnemyKilled);
        ActiveEnemies.Add(Charger);
    }

    // Spawn kamikazes
    for (FVector Loc : KamikazePositions)
    {
        AMechaKamikazeEnemy* Kamikaze = Cast<AMechaKamikazeEnemy>(
            Factory->CreateEnemy(EMechaEnemyType::MechaKamikaze, Loc, FRotator::ZeroRotator));
        if (!Kamikaze) continue;
        Kamikaze->OnEnemyDeath.AddDynamic(this, &ALevel21Facade::OnEnemyKilled);
        ActiveEnemies.Add(Kamikaze);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Wave spawneada: %d enemigos"), ActiveEnemies.Num());
}


void ALevel21Facade::OnEnemyKilled(AEnemyBase* DeadEnemy)
{
    EnemiesKilled++;
    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Enemigos eliminados: %d / %d"),
        EnemiesKilled, TotalEnemies);

    CheckLevelComplete();
}

void ALevel21Facade::CheckLevelComplete()
{
    if (EnemiesKilled < TotalEnemies) return;
    if (bLevelComplete) return;

    bLevelComplete = true;
    GetWorld()->GetTimerManager().ClearTimer(LevelTimer);

    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Nivel completado!"));
    OpenPortal(!bPlayerDied); // si no murio ? secreto, si murio ? jefe
}

void ALevel21Facade::OnTimerExpired()
{
    if (bLevelComplete) return;

    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Tiempo agotado — reintento"));

    // Reiniciamos el nivel
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ALevel21Facade::OnPlayerDeath()
{
    bPlayerDied = true;
    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Jugador murio — no accede al secreto"));
}

void ALevel21Facade::OpenPortal(bool bToSecret)
{
    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("[Level21Facade] GameInstance no encontrado"));
        return;
    }

    if (bToSecret)
    {
        GI->Level2State = ELevelState::Secret;
        UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Estado ? Secret"));
    }
    else
    {
        GI->Level2State = ELevelState::Boss;
        UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Estado ? Boss"));
    }

    if (PortalToBoss)
    {
        PortalToBoss->TargetLevelName = FName("Map_02Boss");
        //PortalToBoss->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalToBoss->SetActorHiddenInGame(false);
    }
    else
        UE_LOG(LogTemp, Error, TEXT("[Level21Facade] Portal no asignado"));
}