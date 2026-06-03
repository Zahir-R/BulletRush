#include "VaultKeeper/core/Level2SFacade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/enemies/VaultKeeper.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameInstance.h"
#include "Components/HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALevel2SFacade::ALevel2SFacade()
{
    PrimaryActorTick.bCanEverTick = false;

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

void ALevel2SFacade::BeginPlay()
{
    Super::BeginPlay();

    Factory = GetWorld()->SpawnActor<AMechaEnemyFactory>(
        AMechaEnemyFactory::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator
    );

    //StartLevel();
    PortalToBoss = GetWorld()->SpawnActor<ALevelPortal>(
        ALevelPortal::StaticClass(),
        FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
    PortalToBoss->SetActorHiddenInGame(true);
}

void ALevel2SFacade::StartLevel()
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    if (Player && Player->HealthComp)
        Player->HealthComp->OnDeath.AddDynamic(this, &ALevel2SFacade::OnPlayerDeath);

    // Veneno cada segundo
    GetWorld()->GetTimerManager().SetTimer(
        VenomTimer,
        this,
        &ALevel2SFacade::ApplyVenom,
        1.0f,
        true
    );

    SpawnWave();
}

void ALevel2SFacade::SpawnWave()
{
    if (!Factory) return;

    for (int32 i = 0; i < TotalDrones; i++)
    {
        FVector SpawnLoc = DroneSpawnLocations.IsValidIndex(i)
            ? DroneSpawnLocations[i]
            : FVector(i * 200.f, 0.f, 100.f);

        ADronMecha* Drone = Cast<ADronMecha>(
            Factory->CreateEnemy(EMechaEnemyType::DroneMecha, SpawnLoc, FRotator::ZeroRotator));

        if (!Drone) continue;

        // Doble balas
        //Drone->BulletMultiplier = 2.0f;

        FVector BatteryLoc = SpawnLoc + FVector(BatteryOffset, 0.f, 0.f);
        ABatteryActor* Battery = Cast<ABatteryActor>(
            Factory->CreateEnemy(EMechaEnemyType::BatteryActor, BatteryLoc, FRotator::ZeroRotator));

        if (Battery)
        {
            Battery->LinkDrone(Drone);
            Drone->LinkBattery(Battery);
        }

        Drone->OnEnemyDeath.AddDynamic(this, &ALevel2SFacade::OnDroneKilled);
        ActiveDrones.Add(Drone);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Wave spawneada con doble balas"));
}

void ALevel2SFacade::ApplyVenom()
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    if (Player && Player->HealthComp && !Player->HealthComp->bDead)
    {
        Player->HealthComp->CurrentHealth -= VenomDamagePerSecond;
        UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Veneno aplicado. Vida: %f"),
            Player->HealthComp->CurrentHealth);

        if (Player->HealthComp->CurrentHealth <= 0.f)
        {
            Player->HealthComp->CurrentHealth = 0.f;
            OnPlayerDeath();
        }
    }
}

// ---------------------------------------------------------
// DRONE KILLED
// ---------------------------------------------------------
void ALevel2SFacade::OnDroneKilled(AEnemyBase* DeadEnemy)
{
    DronesKilled++;
    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Drones: %d / %d"),
        DronesKilled, TotalDrones);
    CheckLevelComplete();
}

void ALevel2SFacade::CheckLevelComplete()
{
    if (DronesKilled < TotalDrones) return;
    if (bLevelComplete) return;

    bLevelComplete = true;
    GetWorld()->GetTimerManager().ClearTimer(VenomTimer);

    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (GI)
    {
        GI->bVaultKeeperWeakened = true;
        GI->Level2State = ELevelState::Boss;
        UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Completado — VK debilitado, estado ? Boss"));
    }

    OpenPortal();
}

void ALevel2SFacade::OpenPortal()
{
    if (PortalToBoss)
    {
        PortalToBoss->TargetLevelName = FName("Map_02Boss");
        PortalToBoss->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalToBoss->SetActorHiddenInGame(false);
    }
    else
        UE_LOG(LogTemp, Error, TEXT("[Level2SFacade] Portal no asignado"));
}

void ALevel2SFacade::OnPlayerDeath()
{
    GetWorld()->GetTimerManager().ClearTimer(VenomTimer);
    UE_LOG(LogTemp, Warning, TEXT("[Level2SFacade] Jugador murio — reintento"));
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}


