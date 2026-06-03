#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/VaultKeeper.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Core/BulletRushGameInstance.h"
#include "Map/LevelPortal.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AVaultKeeperFacade::AVaultKeeperFacade()
{
    PrimaryActorTick.bCanEverTick = false;

    DroneSpawnLocations = {
        FVector(400.f,  400.f, 100.f),
        FVector(-400.f, 400.f, 100.f),
        FVector(400.f, -400.f, 100.f),
        FVector(-400.f,-400.f, 100.f),
    };
}

void AVaultKeeperFacade::BeginPlay()
{
    Super::BeginPlay();

    Factory = GetWorld()->SpawnActor<AMechaEnemyFactory>(
        AMechaEnemyFactory::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator
    );

    //StartLevel();

    PortalToMap = GetWorld()->SpawnActor<ALevelPortal>(
        ALevelPortal::StaticClass(),
        FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
    PortalToMap->SetActorHiddenInGame(true);
}

void AVaultKeeperFacade::StartLevel()
{
    SpawnBoss();

    // Drones cada 30s
    GetWorld()->GetTimerManager().SetTimer(
        DroneSpawnTimer,
        this,
        &AVaultKeeperFacade::SpawnDroneWave,
        DroneSpawnInterval,
        true
    );
}

void AVaultKeeperFacade::SpawnBoss()
{
    if (!Factory) return;

    BossInstance = Cast<AVaultKeeper>(
        Factory->CreateEnemy(EMechaEnemyType::VaultKeeper,
            BossSpawnLocation, FRotator::ZeroRotator));

    if (!BossInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("[VaultKeeperFacade] No se pudo spawnear el VaultKeeper"));
        return;
    }

    // Verificamos recompensa del 2-S
    
    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (GI && GI->bVaultKeeperWeakened)
    {
        BossInstance->DestroyOneWeakPoint();
        GI->bVaultKeeperWeakened = false; // reseteamos para proxima partida
        UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] VaultKeeper debilitado por recompensa 2-S"));
    }

    // Suscribimos muerte del boss
    BossInstance->OnEnemyDeath.AddDynamic(this, &AVaultKeeperFacade::OnBossDeath);

    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] VaultKeeper spawneado"));
}

void AVaultKeeperFacade::SpawnDroneWave()
{
    if (!Factory || bLevelComplete) return;

    for (int32 i = 0; i < DronesPerSpawn; i++)
    {
        // Rotamos las posiciones disponibles
        int32 Index = (DroneSpawnCount + i) % DroneSpawnLocations.Num();
        FVector SpawnLoc = DroneSpawnLocations[Index];

        ADronMecha* Drone = Cast<ADronMecha>(
            Factory->CreateEnemy(EMechaEnemyType::DroneMecha,
                SpawnLoc, FRotator::ZeroRotator));

        if (!Drone) continue;

        // Bateria vinculada
        FVector BatteryLoc = SpawnLoc + FVector(150.f, 0.f, 0.f);
        ABatteryActor* Battery = Cast<ABatteryActor>(
            Factory->CreateEnemy(EMechaEnemyType::BatteryActor,
                BatteryLoc, FRotator::ZeroRotator));

        if (Battery)
        {
            Battery->LinkDrone(Drone);
            Drone->LinkBattery(Battery);
        }

        Drone->OnEnemyDeath.AddDynamic(this, &AVaultKeeperFacade::OnDroneKilled);
        ActiveDrones.Add(Drone);
    }

    DroneSpawnCount += DronesPerSpawn;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Drones spawneados: %d"), DronesPerSpawn);
}

void AVaultKeeperFacade::OnBossDeath(AEnemyBase* DeadEnemy)
{
    if (bLevelComplete) return;
    bLevelComplete = true;

    GetWorld()->GetTimerManager().ClearTimer(DroneSpawnTimer);

    for (ADronMecha* Drone : ActiveDrones)
        if (Drone && IsValid(Drone))
            Drone->Destroy();
    ActiveDrones.Empty();

    // Marcamos el mapa como completado
    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (GI)
    {
        GI->MarcarMapaCompletado(FName("Map_02Boss"));
        GI->Level2State = ELevelState::Normal; // reseteamos para proxima vez
        UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Nivel completo — mapa marcado"));
    }

    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Boss derrotado"));
    OpenPortal();
}

void AVaultKeeperFacade::OpenPortal()
{
    if (PortalToMap)
    {
        PortalToMap->TargetLevelName = FName("Map_CupHeadMap");
        PortalToMap->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalToMap->SetActorHiddenInGame(false);
        UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Portal al mapa abierto"));
    }
    else
        UE_LOG(LogTemp, Error, TEXT("[VaultKeeperFacade] PortalToMap no asignado"));
}

void AVaultKeeperFacade::OnDroneKilled(AEnemyBase* DeadEnemy)
{
    ADronMecha* Drone = Cast<ADronMecha>(DeadEnemy);
    if (Drone) ActiveDrones.Remove(Drone);
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Drone eliminado. Activos: %d"),
        ActiveDrones.Num());
}

