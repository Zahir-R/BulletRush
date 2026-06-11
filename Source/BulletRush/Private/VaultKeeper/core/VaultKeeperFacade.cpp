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
#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/BulletRushHUD.h"

AVaultKeeperFacade::AVaultKeeperFacade()
{
    PrimaryActorTick.bCanEverTick = false;

    DroneSpawnLocations = {
        FVector(400.f,  400.f, 100.f),
        FVector(-400.f, 400.f, 100.f),
        FVector(400.f, -400.f, 100.f),
        FVector(-400.f,-400.f, 100.f),
    };

    static ConstructorHelpers::FObjectFinder<USoundBase> CombatFinder(TEXT("SoundWave'/Game/Audio/1-_Brave_reaction.1-_Brave_reaction'"));
    if (CombatFinder.Succeeded()) CombatSong = CombatFinder.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
    if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;
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
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
    {
        float StartTime = CombatStartOffset;
        if (Music->IsPositionSaved()) StartTime = Music->ConsumeSavedPosition();
        Music->PlaySong(CombatSong, StartTime, 2.0f, true);
    }

    SpawnBoss();

    // Drones cada 30s
    GetWorld()->GetTimerManager().SetTimer(
        DroneSpawnTimer,
        this,
        &AVaultKeeperFacade::SpawnDroneWave,
        DroneSpawnInterval,
        true
    );

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
    if (HUD)
    {
        HUD->SetObjective(TEXT("Derrota al VaultKeeper"));
    }
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

    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
        Music->TransitionTo(AmbientSong, 3.0f, 0.5f, 0.0f);

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
        PortalToMap->bIsExitPortal = true;
        PortalToMap->EvaluarEstadoDeDesbloqueo();
        PortalToMap->RequiredLevelToUnlock = NAME_None;
        PortalToMap->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        PortalToMap->SetActorHiddenInGame(false);
        UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Portal al mapa abierto"));
    }
    else
        UE_LOG(LogTemp, Error, TEXT("[VaultKeeperFacade] PortalToMap no asignado"));
}



