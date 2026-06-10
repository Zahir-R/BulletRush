#include "VaultKeeper/core/Level21Facade.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "VaultKeeper/enemies/MechaKamikazeEnemy.h"
#include "VaultKeeper/enemies/MechaChargerEnemy.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/Requirements/RequirementManager.h"
#include "Core/Requirements/NoPowerUpRequirement.h"
#include "Core/PowerUpUsagePublisher.h"
#include "Components/HealthComponent.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ALevel21Facade::ALevel21Facade()
{
    PrimaryActorTick.bCanEverTick = false;

    static ConstructorHelpers::FObjectFinder<USoundBase> CombatFinder(TEXT("SoundWave'/Game/Audio/1-_Brave_reaction.1-_Brave_reaction'"));
    if (CombatFinder.Succeeded()) CombatSong = CombatFinder.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
    if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;

    // Spawn locations por defecto si no se asignan en el editor
    DroneSpawnLocations = {
        FVector(580.f,    800.f, 100.f),
        FVector(-1500.f,   0.f, 700.f),
        FVector(2150.f,    1500.f, 400.f),
        FVector(0.f,   -500.f, 200.f),
        FVector(2700.f,  1300.f, 500.f),
        FVector(-700.f, 1300.f, 900.f),
        FVector(300.f,  1700.f, 600.f),
        FVector(-2300.f,-700.f, 300.f),
        FVector(900.f,    100.f, 150.f),
        FVector(-1900.f,   0.f, 900.f),
        FVector(0.f,    900.f, 1050.f),
        FVector(0.f,   -900.f, 1700.f),
        FVector(600.f, -600.f, 100.f),
        FVector(-600.f, 3000.f, 1000.f),
        FVector(1000.f, 400.f, 1500.f),
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

    PowerUpPublisher = GetWorld()->SpawnActor<APowerUpUsagePublisher>(
        APowerUpUsagePublisher::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator);
}


void ALevel21Facade::StartLevel()
{
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
    {
        float StartTime = CombatStartOffset;
        if (Music->IsPositionSaved())
        {
            StartTime = Music->ConsumeSavedPosition();
        }
        Music->PlaySong(CombatSong, StartTime, 2.0f, true);
    }

    // Suscribimos la muerte del jugador
    APlayingPlayer* Player = Cast<APlayingPlayer>(
        UGameplayStatics::GetPlayerPawn(this, 0));

    // RequirementManager: detecta si el jugador uso power-ups
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        URequirementManager* ReqMgr = NewObject<URequirementManager>(PC);
        ReqMgr->RegisterComponent();

        UNoPowerUpRequirement* NoPowerUpReq = NewObject<UNoPowerUpRequirement>(ReqMgr);
        ReqMgr->SecretRequirements.Add(NoPowerUpReq);

        ReqMgr->InitializeRequirements(PC);
        RequirementManagerRef = ReqMgr;
    }

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
        FVector(0.f,   3900.f, 100.f), FVector(0.f,   -1900.f, 100.f),
    };

    // 3 Kamikazes
    TArray<FVector> KamikazePositions = {
        FVector(3600.f, -600.f, 100.f),
        FVector(-600.f, 600.f, 3100.f),
        FVector(1000.f, 3400.f, 100.f),
    };

    // Spawn drones
    
    for (FVector Loc : DroneSpawnLocations)
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
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
        Music->TransitionTo(AmbientSong, 3.0f, 0.5f, 0.0f);
    bool bSecretUnlocked = RequirementManagerRef.IsValid() && RequirementManagerRef->AreSecretRequirementsMet();
    OpenPortal(bSecretUnlocked);
}

void ALevel21Facade::OnTimerExpired()
{
    if (bLevelComplete) return;

    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Tiempo agotado � reintento"));

    // Reiniciamos el nivel
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ALevel21Facade::OnPlayerDeath()
{
    bPlayerDied = true;
    UE_LOG(LogTemp, Warning, TEXT("[Level21Facade] Jugador murio � no accede al secreto"));
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