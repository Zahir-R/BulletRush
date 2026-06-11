#include "Core/Bloodseeker/BloodseekerGameMode.h"
#include "Core/Bloodseeker/BloodseekerFacade.h"
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Enemies/EnemyBase.h"
#include "Map/BossSpawnTrigger.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameInstance.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Engine/World.h"
#include "Enemies/Bloodseeker/SkySphereWorld.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ABloodseekerGameMode::ABloodseekerGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
    Facade = nullptr;
    BossRef = nullptr;
    BossTrigger = nullptr;
    bBossDied = false;
    SpawnBossTriggerLocation = FVector(900.0f, 0.0f, 100.0f);

    static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
    if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;
}

void ABloodseekerGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("[BloodseekerGameMode] BeginPlay ejecutado"));
    
    if (GetWorld()) {
        SkySphere = GetWorld()->SpawnActor<ASkySphereWorld>(ASkySphereWorld::StaticClass(), FTransform(FVector::ZeroVector));
    }
    bBossDied = false;

    Facade = GetWorld()->SpawnActor<ABloodseekerFacade>(
        FacadeClass ? FacadeClass : ABloodseekerFacade::StaticClass(),
        FTransform(FVector(0.0f, 0.0f, 0.0f))
    );

    if (Facade)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BloodseekerGameMode] Facade creado exitosamente, iniciando juego..."));
        Facade->OnAllWavesComplete.AddDynamic(this, &ABloodseekerGameMode::OnAllWavesComplete);
        Facade->StartGame();
        //suscribirse a muerte del ugador
        APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
        if (Player && Player->HealthComp)
            Player->HealthComp->OnDeath.AddDynamic(this, &ABloodseekerGameMode::OnPlayerDeath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BloodseekerGameMode] ERROR: No se pudo crear el Facade!"));
    }
}

void ABloodseekerGameMode::OnAllWavesComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("[BloodseekerGameMode] OnAllWavesComplete - Todas las oleadas terminadas, spawneando BossTrigger..."));
    if (!GetWorld()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    BossTrigger = GetWorld()->SpawnActor<ABossSpawnTrigger>(
        BossTriggerClass ? BossTriggerClass : ABossSpawnTrigger::StaticClass(),
        FTransform(SpawnBossTriggerLocation),
        SpawnParams
    );

    if (BossTrigger)
    {
        BossTrigger->OnBossSpawned.AddDynamic(this, &ABloodseekerGameMode::OnBossSpawned);
    }
}

void ABloodseekerGameMode::OnBossSpawned(ABloodseekerBoss* SpawnedBoss)
{
    UE_LOG(LogTemp, Warning, TEXT("[BloodseekerGameMode] OnBossSpawned llamado - Boss: %s"), SpawnedBoss ? *SpawnedBoss->GetName() : TEXT("NULL"));
    if (SpawnedBoss)
    {
        BossRef = SpawnedBoss;
        BossRef->FacadeRef = Facade;
        //nivel-s
        if (Facade->bSecretLevel)
        {
            float OriginalMax = BossRef->HealthComp->MaxHealth;
            BossRef->HealthComp->MaxHealth = OriginalMax * 0.75f;
            BossRef->HealthComp->CurrentHealth = BossRef->HealthComp->MaxHealth;
            UE_LOG(LogTemp, Warning, TEXT("[GameMode] SECRETO! Boss con 25%% menos HP: %.0f"), BossRef->HealthComp->MaxHealth);
        }
        //
        BossRef->OnEnemyDeath.AddDynamic(this, &ABloodseekerGameMode::OnBossDied);

        if (Facade)
        {
            Facade->StartBossPhase();
        }
    }
}

void ABloodseekerGameMode::OnBossDied(AEnemyBase* DeadBoss)
{
    UE_LOG(LogTemp, Warning, TEXT("[BloodseekerGameMode] OnBossDied llamado - Boss: %s"), DeadBoss ? *DeadBoss->GetName() : TEXT("NULL"));
    if (bBossDied) return;
    bBossDied = true;

    if (Facade)
    {
        Facade->StopAllSpawning();
    }

    CleanupLevel();

    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
    {
        Music->TransitionTo(AmbientSong, 3.0f, 0.5f, 0.0f);
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UProjectilesSubsystem* ProjSys = GameInstance->GetSubsystem<UProjectilesSubsystem>();
        if (ProjSys)
        {
            ProjSys->ReturnAllActiveBullets();
        }

        UBulletRushGameInstance* BRGI = Cast<UBulletRushGameInstance>(GameInstance);
        if (BRGI)
        {
            BRGI->MarcarMapaCompletado(FName("Nivel_Boss1"));
        }
    }
}

void ABloodseekerGameMode::CleanupLevel()
{
    if (!GetWorld()) return;

    if (Facade)
    {
        Facade->DestroyAllEnemies();
    }

    if (BossTrigger && IsValid(BossTrigger))
    {
        BossTrigger->Destroy();
        BossTrigger = nullptr;
    }
}
//BRGI
void ABloodseekerGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (Player && Player->HealthComp)
        Player->HealthComp->OnDeath.RemoveDynamic(this, &ABloodseekerGameMode::OnPlayerDeath);
    Super::EndPlay(EndPlayReason);
}
void ABloodseekerGameMode::OnPlayerDeath()
{
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
        Music->NotifyLevelTravel();

    UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
    if (!GI) return;

    FName MapName = FName(*GetWorld()->GetName());
    int32 VidasRestantes = GI->DecrementarVida(MapName);

    if (VidasRestantes > 0)
    {
        UGameplayStatics::OpenLevel(this, MapName);
    }
    else
    {
        // Sin vidas
        GI->ResetVidas(MapName);
        UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
    }
}

