#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/VaultKeeper.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "VaultKeeper/enemies/MechaKamikazeEnemy.h"
#include "VaultKeeper/enemies/MechaChargerEnemy.h"
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
        int32 Index = (DroneSpawnCount + i) % DroneSpawnLocations.Num();
        FVector SpawnLoc = DroneSpawnLocations[Index];

        // Enemigo aleatorio entre los 3 tipos
        int32 RandType = FMath::RandRange(0, 2);
        AEnemyBase* SpawnedEnemy = nullptr;

        switch (RandType)
        {
        case 0:
            SpawnedEnemy = Cast<AEnemyBase>(
                Factory->CreateEnemy(EMechaEnemyType::DroneMecha,
                    SpawnLoc, FRotator::ZeroRotator));
            break;
        case 1:
            SpawnedEnemy = Cast<AEnemyBase>(
                Factory->CreateEnemy(EMechaEnemyType::MechaCharger,
                    SpawnLoc, FRotator::ZeroRotator));
            break;
        case 2:
            SpawnedEnemy = Cast<AEnemyBase>(
                Factory->CreateEnemy(EMechaEnemyType::MechaKamikaze,
                    SpawnLoc, FRotator::ZeroRotator));
            break;
        }

        if (!SpawnedEnemy) continue;
        SpawnedEnemy->OnEnemyDeath.AddDynamic(this, &AVaultKeeperFacade::OnDroneKilled);
        ActiveEnemies.Add(SpawnedEnemy); // Cast puede ser null para Charger/Kamikaze
    }

    DroneSpawnCount += DronesPerSpawn;
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Enemigos spawneados: %d"), DronesPerSpawn);
}

void AVaultKeeperFacade::OnBossDeath(AEnemyBase* DeadEnemy)
{
    if (bLevelComplete) return;
    bLevelComplete = true;

    GetWorld()->GetTimerManager().ClearTimer(DroneSpawnTimer);

    for (AEnemyBase* Enemy : ActiveEnemies)
        if (Enemy && IsValid(Cast<UObject>(Enemy)))
            Enemy->Destroy();
    ActiveEnemies.Empty();

    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (GI)
    {
        GI->MarcarMapaCompletado(FName("Map_02Boss"));
        GI->Level2State = ELevelState::Normal;
    }

    OpenPortal();
}

void AVaultKeeperFacade::OnDroneKilled(AEnemyBase* DeadEnemy)
{
    ActiveEnemies.Remove(DeadEnemy);
    UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Enemigo eliminado. Activos: %d"),
        ActiveEnemies.Num());
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



