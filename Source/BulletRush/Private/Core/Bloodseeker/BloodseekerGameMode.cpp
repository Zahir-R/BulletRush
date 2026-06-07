#include "Core/Bloodseeker/BloodseekerGameMode.h"
#include "Core/Bloodseeker/BloodseekerFacade.h"
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Enemies/EnemyBase.h"
#include "Map/BossSpawnTrigger.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameInstance.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Enemies/Bloodseeker/SkySphereWorld.h"
#include "Kismet/GameplayStatics.h"

ABloodseekerGameMode::ABloodseekerGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
    Facade = nullptr;
    BossRef = nullptr;
    BossTrigger = nullptr;
    bBossDied = false;
    SpawnBossTriggerLocation = FVector(900.0f, 0.0f, 100.0f);
 
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


