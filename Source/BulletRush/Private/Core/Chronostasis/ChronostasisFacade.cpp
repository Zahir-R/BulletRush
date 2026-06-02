#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Chronostasis/GameModeChronostasis.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "Core/Chronostasis/DroneFactory.h"
#include "Core/Chronostasis/MassFactory.h"
#include "Core/Chronostasis/ExpansiveFactory.h"
#include "Core/Chronostasis/ChargerFactory.h"
#include "Core/Chronostasis/LinkerFactory.h"
#include "Core/BulletRushGameModeBase.h"
#include "Enemies/EnemyBase.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Components/BuffComponent.h"
#include "Buffs/TimeSlowDecorator.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"
#include "Core/BulletRushHUD.h"
#include "Core/Requirements/RequirementManager.h"
#include "Subsystems/ProjectilesSubsystem.h"

ABulletRushHUD* AChronostasisFacade::GetHUD() const
{
	APlayerController* PC = OwningGameMode ? UGameplayStatics::GetPlayerController(OwningGameMode->GetWorld(), 0) : nullptr;
	return PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
}

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
    DroneFactory = NewObject<UDroneFactory>(this);
    MassFactory = NewObject<UMassFactory>(this);
    ExpansiveFactory = NewObject<UExpansiveFactory>(this);
    ChargerFactory = NewObject<UChargerFactory>(this);
    LinkerFactory = NewObject<ULinkerFactory>(this);
    BossChargerFactory = NewObject<UChargerFactory>(this);
    BossLinkerFactory = NewObject<ULinkerFactory>(this);

    if (!SerXBossClass)
    {
        SerXBossClass = ASerXBoss::StaticClass();
    }

    // Waves hardcodeadas waos
    if (Waves.Num() == 0)
    {
        FWaveConfig W1; W1.DroneCount = 1; W1.SpawnPoints = { FVector(1000,0,0), FVector(-1000,0,0), FVector(0,1000,0) };
        FWaveConfig W2; W2.DroneCount = 1; W2.MassCount = 1; W2.SpawnPoints = { FVector(200,0,0), FVector(-200,0,0), FVector(0,200,0) };
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
            // TimeSlowDecorator acumulativo
            UE_LOG(LogTemp, Warning, TEXT("Aplicando TimeSlowDecorator al jugador"));
            BuffComp->ApplyBuff(UTimeSlowDecorator::StaticClass(), -1.0f, 0.0f);
        }
    }
    StartSlowTimer();
    NotifySubscribers();
}

void AChronostasisFacade::StartWave(int32 Index)
{
    UE_LOG(LogTemp, Warning, TEXT("Intentando iniciar oleada %d"), Index);
    if (!Waves.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("Indice de oleada %d invalido. Total oleadas: %d"), Index, Waves.Num());
        return;
    }

    CurrentWaveIndex = Index;
    const FWaveConfig& Cfg = Waves[Index];
    UE_LOG(LogTemp, Warning, TEXT("Iniciando oleada %d con Drones: %d, Mass: %d, Expansive: %d"), 
        Index, Cfg.DroneCount, Cfg.MassCount, Cfg.ExpansiveCount);

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("World es nulo"));
        return;
    }
    int32 SpawnedCount = 0;
    int32 SpawnIndex = 0;
    for (int32 i=0;i<Cfg.DroneCount;i++)
    {
        FVector Loc = Cfg.SpawnPoints[SpawnIndex];
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Drone en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = DroneFactory->CreateEnemy(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Drone spawneado con exito"));
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
            SpawnedCount++;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Drone"));
        }
    }
    for (int32 i=0;i<Cfg.MassCount;i++)
    {
        FVector Loc = Cfg.SpawnPoints[SpawnIndex];
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Mass en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = MassFactory->CreateEnemy(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Mass spawneado con exito"));
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
            SpawnedCount++;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Mass"));
        }
    }
    for (int32 i=0;i<Cfg.ExpansiveCount;i++)
    {
        FVector Loc = Cfg.SpawnPoints[SpawnIndex];
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Expansive en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = ExpansiveFactory->CreateEnemy(World, Loc);
        if (E)
        {
            UE_LOG(LogTemp, Warning, TEXT("Expansive spawneado con exito"));
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
            SpawnedCount++;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Expansive"));
        }
    }
    for (int32 i=0;i<Cfg.ChargerCount;i++)
    {
        FVector Loc = Cfg.SpawnPoints[SpawnIndex];
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Charger en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = ChargerFactory->CreateEnemy(World, Loc);
        if (E)
        {
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
            SpawnedCount++;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Charger"));
        }
    }
    for (int32 i=0;i<Cfg.LinkerCount;i++)
    {
        FVector Loc = Cfg.SpawnPoints[SpawnIndex];
        SpawnIndex++;
        UE_LOG(LogTemp, Warning, TEXT("Spawneando Linker en posicion: %s"), *Loc.ToString());
        AEnemyBase* E = LinkerFactory->CreateEnemy(World, Loc);
        if (E)
        {
            E->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnEnemyKilled);
            SpawnedCount++;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Error al spawnear Linker"));
        }
    }
    RemainingEnemiesInWave = SpawnedCount;
}

void AChronostasisFacade::OnEnemyKilled(AEnemyBase* Enemy)
{
    if (RemainingEnemiesInWave <= 0) return;
    RemainingEnemiesInWave--;
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
    GetWorldTimerManager().ClearTimer(SlowTimerHandle);
    GetWorldTimerManager().ClearTimer(SecretCountdownTimerHandle);
    GetWorldTimerManager().ClearTimer(SecretTeleportDelayHandle);
    bSecretTimerPaused = false;

    if (bIsSecretLevel)
    {
        bIsSecretLevel = false;
        if (OwningGameMode)
        {
            UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
            if (ProjSys) ProjSys->SetSecretLevel(false);

            AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
            if (GM) GM->OnSecretLevelCompleted();
        }
        return;
    }

    if (OwningGameMode)
    {
        bool bSecretUnlocked = false;
        if (RequirementManagerRef.IsValid() && RequirementManagerRef->HasRequirements())
        {
            bSecretUnlocked = RequirementManagerRef->AreSecretRequirementsMet();
        }
        else
        {
            bSecretUnlocked = (SlowTriggerCount >= 3) && !bPlayerTookDamage;
        }

        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM)
        {
            GM->ActivateBossPortal();
            if (bSecretUnlocked)
            {
                GM->ActivateSecretPortal();
            }
        }
    }
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

void AChronostasisFacade::SetRequirementManager(URequirementManager* Manager)
{
    RequirementManagerRef = Manager;
}

void AChronostasisFacade::StartSecretWaves(const TArray<FWaveConfig>& NewWaves)
{
    bIsSecretLevel = true;
    Waves = NewWaves;
    CurrentWaveIndex = 0;

    UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
    if (ProjSys) ProjSys->SetSecretLevel(true);

    SecretLevelTimeRemaining = 120.f;
    bSecretTimerPaused = false;

    GetWorldTimerManager().SetTimer(SecretCountdownTimerHandle, this, &AChronostasisFacade::OnSecretCountdownTick, 1.f, true);

    StartWave(0);

    ABulletRushHUD* HUD = GetHUD();
    if (HUD)
    {
        HUD->ShowMessage("SECRET LEVEL", 3.f);
    }
}

void AChronostasisFacade::OnSecretCountdownTick()
{
    if (bSecretTimerPaused) return;

    SecretLevelTimeRemaining -= 1.f;
    ABulletRushHUD* HUD = GetHUD();
    if (HUD)
    {
        HUD->SetCountdown(SecretLevelTimeRemaining);
    }

    if (SecretLevelTimeRemaining <= 0.f)
    {
        GetWorldTimerManager().ClearTimer(SecretCountdownTimerHandle);
        bSecretTimerPaused = true;

        ABulletRushHUD* HUD2 = GetHUD();
        if (HUD2)
        {
            HUD2->ShowMessage("TIME'S UP! Teleporting...", 3.f);
        }

        GetWorldTimerManager().SetTimer(SecretTeleportDelayHandle, this, &AChronostasisFacade::OnSecretTimeUpTeleport, 3.f, false);
    }
}

void AChronostasisFacade::OnSecretTimeUpTeleport()
{
    bIsSecretLevel = false;
    UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
    if (ProjSys) ProjSys->SetSecretLevel(false);

    if (OwningGameMode)
    {
        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM) GM->OnSecretLevelCompleted();
    }
}

void AChronostasisFacade::StartBossFight()
{
    bIsBossFight = true;

    GetWorldTimerManager().ClearTimer(SlowTimerHandle);
    GetWorldTimerManager().ClearTimer(SecretCountdownTimerHandle);
    GetWorldTimerManager().ClearTimer(SecretTeleportDelayHandle);

    UWorld* World = GetWorld();
    if (!World || !SerXBossClass)
    {
        UE_LOG(LogTemp, Error, TEXT("AChronostasisFacade::StartBossFight: World or SerXBossClass is null"));
        return;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ASerXBoss* Boss = World->SpawnActor<ASerXBoss>(SerXBossClass, BossArenaSpawnLocation, FRotator::ZeroRotator, Params);
    if (Boss)
    {
        Boss->OnEnemyDeath.AddDynamic(this, &AChronostasisFacade::OnBossKilled);
        Boss->SetChargerFactory(BossChargerFactory);
        Boss->SetLinkerFactory(BossLinkerFactory);

        ABulletRushHUD* HUD = GetHUD();
        if (HUD)
        {
            HUD->ShowMessage(TEXT("BOSS FIGHT"), 3.f);
        }
        UE_LOG(LogTemp, Log, TEXT("AChronostasisFacade::StartBossFight: SerXBoss spawned at %s"), *BossArenaSpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AChronostasisFacade::StartBossFight: Failed to spawn SerXBoss"));
    }
}

void AChronostasisFacade::OnBossKilled(AEnemyBase* Boss)
{
    bIsBossFight = false;

    ABulletRushHUD* HUD = GetHUD();
    if (HUD)
    {
        HUD->ShowMessage(TEXT("VICTORY"), 5.f);
    }

    UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
    if (ProjSys) ProjSys->ReturnAllActiveBullets();

    UE_LOG(LogTemp, Log, TEXT("AChronostasisFacade::OnBossKilled: Boss defeated!"));
}

void AChronostasisFacade::OnBossPortalTriggered()
{
    StartBossFight();
}
