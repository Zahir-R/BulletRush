#include "VaultKeeper/core/Level21Facade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
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
}


void ALevel21Facade::StartLevel()
{
    // Suscribimos la muerte del jugador
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    if (Player && Player->HealthComp)
        Player->HealthComp->OnDeath.AddDynamic(this, &ALevel21Facade::OnPlayerDeath);

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

    for (int32 i = 0; i < TotalDrones; i++)
    {
        FVector SpawnLoc = DroneSpawnLocations.IsValidIndex(i)
            ? DroneSpawnLocations[i]
            : FVector(i * 200.f, 0.f, 100.f);

        // Spawn drone
        ADronMecha* Drone = Cast<ADronMecha>(
            Factory->CreateEnemy(EMechaEnemyType::DroneMecha, SpawnLoc, FRotator::ZeroRotator));

        if (!Drone) continue;

        // Spawn bateria cerca del drone
        FVector BatteryLoc = SpawnLoc + FVector(BatteryOffset, 0.f, 0.f);
        ABatteryActor* Battery = Cast<ABatteryActor>(
            Factory->CreateEnemy(EMechaEnemyType::BatteryActor, BatteryLoc, FRotator::ZeroRotator));

        // Vinculamos drone y bateria
        if (Battery)
        {
            Battery->LinkDrone(Drone);
            Drone->LinkBattery(Battery);
        }

        // Suscribimos la muerte del drone
        Drone->OnEnemyDeath.AddDynamic(this, &ALevel21Facade::OnDroneKilled);
        ActiveDrones.Add(Drone);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Wave spawneada: %d drones"), TotalDrones);
}


void ALevel21Facade::OnDroneKilled(AEnemyBase* DeadEnemy)
{
    DronesKilled++;
    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Drones eliminados: %d / %d"),
        DronesKilled, TotalDrones);

    CheckLevelComplete();
}

void ALevel21Facade::CheckLevelComplete()
{
    if (DronesKilled < TotalDrones) return;
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