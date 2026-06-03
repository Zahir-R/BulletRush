#include "Core/Chronostasis/GameModeChronostasis.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Requirements/RequirementManager.h"
#include "Core/BulletRushHUD.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/PlayerHealthPublisher.h"
#include "Core/CollectiblePickupPublisher.h"
#include "Core/PowerUpUsagePublisher.h"
#include "Core/PuzzleEventPublisher.h"
#include "Map/PortalTrigger.h"

AGameModeChronostasis::AGameModeChronostasis()
{
    HUDClass = ABulletRushHUD::StaticClass();

    // Define secret level waves
    FWaveConfig SW1; SW1.DroneCount = 4; SW1.ChargerCount = 2; SW1.SpawnPoints = { FVector(500,0,0), FVector(-500,0,0), FVector(0,500,0), FVector(0,-500,0), FVector(700,300,0), FVector(-700,-300,0) };
    FWaveConfig SW2; SW2.MassCount = 2; SW2.LinkerCount = 1; SW2.SpawnPoints = { FVector(300,300,0), FVector(-300,-300,0), FVector(400,-400,0) };
    FWaveConfig SW3; SW3.DroneCount = 3; SW3.MassCount = 1; SW3.ChargerCount = 2; SW3.LinkerCount = 1; SW3.SpawnPoints = { FVector(200,0,0), FVector(-200,0,0), FVector(0,200,0), FVector(0,-200,0), FVector(400,0,0), FVector(-400,0,0), FVector(300,-300,0) };
    SecretWaves = { SW1, SW2, SW3 };
}

void AGameModeChronostasis::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Iniciando GameMode"));
    Super::BeginPlay();
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("AGameModeChronostasis::BeginPlay: World es NULO"));
        return;
    }
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AChronostasisFacade::StaticClass(), Found);
    AChronostasisFacade* Facade = nullptr;
    if (Found.Num() > 0)
    {
        Facade = Cast<AChronostasisFacade>(Found[0]);
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Se encontro un Facade existente en la escena: %s"), *Facade->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: No se encontro Facade en escena, procediendo a spawnear uno nuevo en (0,0,0)"));
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Facade = World->SpawnActor<AChronostasisFacade>(AChronostasisFacade::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    }
    if (Facade)
    {
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Llamando a Facade->StartGame()"));

        // Spawn publishers for the Observer pattern (plain Observer, not UE4 delegates)
        FActorSpawnParameters PubParams;
        PubParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        World->SpawnActor<APlayerHealthPublisher>(APlayerHealthPublisher::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, PubParams);
        World->SpawnActor<ACollectiblePickupPublisher>(ACollectiblePickupPublisher::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, PubParams);
        World->SpawnActor<APowerUpUsagePublisher>(APowerUpUsagePublisher::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, PubParams);
        World->SpawnActor<APuzzleEventPublisher>(APuzzleEventPublisher::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, PubParams);

        // Find RequirementManager on the player pawn or spawn one
        URequirementManager* ReqMgr = nullptr;
        APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
        if (PC)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                ReqMgr = Pawn->FindComponentByClass<URequirementManager>();
            }
            if (!ReqMgr)
            {
                ReqMgr = NewObject<URequirementManager>(PC);
                ReqMgr->RegisterComponent();
            }
            ReqMgr->InitializeRequirements(PC);
        }
        Facade->SetRequirementManager(ReqMgr);
        CachedFacade = Facade;
        Facade->StartGame();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AGameModeChronostasis::BeginPlay: Error al obtener o spawnear el Facade"));
    }
}

void AGameModeChronostasis::ActivateSecretPortal()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UClass* PortalClass = SecretPortalClass ? SecretPortalClass.Get() : APortalTrigger::StaticClass();
    APortalTrigger* Portal = World->SpawnActor<APortalTrigger>(PortalClass, SecretPortalSpawnLocation, FRotator::ZeroRotator, Params);
    if (Portal)
    {
        Portal->bIsActive = true;
        Portal->OnPortalTriggered.Clear();
        Portal->OnPortalTriggered.AddUObject(this, &AGameModeChronostasis::OnSecretPortalTriggered);
        SpawnedSecretPortal = Portal;

        UE_LOG(LogTemp, Log, TEXT("ActivateSecretPortal: Secret portal spawned at %s"), *SecretPortalSpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ActivateSecretPortal: Failed to spawn secret portal!"));
    }

}

void AGameModeChronostasis::OnSecretPortalTriggered()
{
    if (bSecretLevelTriggered) return;
    bSecretLevelTriggered = true;

    UE_LOG(LogTemp, Log, TEXT("OnSecretPortalTriggered: Player entered secret portal! Starting secret level."));

    // Deactivate portal immediately to prevent re-triggering
    if (SpawnedSecretPortal)
    {
        SpawnedSecretPortal->bIsActive = false;
    }

    URequirementManager* ReqMgr = nullptr;
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        APawn* Pawn = PC->GetPawn();
        if (Pawn) ReqMgr = Pawn->FindComponentByClass<URequirementManager>();
    }

    StartSecretLevel(ReqMgr);

    // Destroy portal after use
    if (SpawnedSecretPortal)
    {
        SpawnedSecretPortal->Destroy();
        SpawnedSecretPortal = nullptr;
    }
}

void AGameModeChronostasis::ActivateBossPortal()
{
    if (SpawnedBossPortal && IsValid(SpawnedBossPortal))
    {
        SpawnedBossPortal->bIsActive = true;
        UE_LOG(LogTemp, Log, TEXT("ActivateBossPortal: Boss portal already present, reactivated."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UClass* PortalClass = BossPortalClass ? BossPortalClass.Get() : APortalTrigger::StaticClass();
    APortalTrigger* Portal = World->SpawnActor<APortalTrigger>(PortalClass, BossPortalSpawnLocation, FRotator::ZeroRotator, Params);
    if (Portal)
    {
        Portal->bIsActive = true;
        Portal->OnPortalTriggered.Clear();
        Portal->OnPortalTriggered.AddUObject(this, &AGameModeChronostasis::OnBossPortalTriggered);
        SpawnedBossPortal = Portal;

        UE_LOG(LogTemp, Log, TEXT("ActivateBossPortal: Boss portal spawned at %s"), *BossPortalSpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ActivateBossPortal: Failed to spawn boss portal!"));
    }
}

void AGameModeChronostasis::OnBossPortalTriggered()
{
    if (bBossPortalTriggered) return;
    bBossPortalTriggered = true;

    if (SpawnedBossPortal)
    {
        SpawnedBossPortal->bIsActive = false;
    }

    if (CachedFacade)
    {
        CachedFacade->OnBossPortalTriggered();
    }

    if (SpawnedBossPortal)
    {
        SpawnedBossPortal->Destroy();
        SpawnedBossPortal = nullptr;
    }
}

void AGameModeChronostasis::StartSecretLevel(URequirementManager* RequirementManager)
{
    if (!CachedFacade)
    {
        UE_LOG(LogTemp, Error, TEXT("AGameModeChronostasis::StartSecretLevel: No facade cached!"));
        return;
    }

    if (SpawnedBossPortal && IsValid(SpawnedBossPortal))
    {
        SpawnedBossPortal->bIsActive = false;
        UE_LOG(LogTemp, Log, TEXT("StartSecretLevel: Boss portal deactivated for secret level."));
    }

    CachedFacade->SetRequirementManager(RequirementManager);
    CachedFacade->StartSecretWaves(SecretWaves);
}

void AGameModeChronostasis::OnSecretLevelCompleted()
{
    bool bWavesCleared = CachedFacade && CachedFacade->AreAllWavesComplete();

    if (!bWavesCleared)
    {
        // Time ran out — no reward
        UE_LOG(LogTemp, Warning, TEXT("Secret level failed (time expired). Teleporting to boss without reward."));
    }
    else
    {
        // Reward: apply cooldown multiplier to GameInstance
        UBulletRushGameInstance* GI = GetGameInstance<UBulletRushGameInstance>();
        if (GI)
        {
            GI->PowerUpCooldownMultiplier = 0.5f;
        }
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
        if (HUD)
        {
            HUD->ShowMessage("REWARD: PowerUp cooldown halved!", 5.f);
        }
        UE_LOG(LogTemp, Warning, TEXT("Secret level completed! Reward granted."));
    }

    bSecretLevelTriggered = false;

    if (SpawnedBossPortal && IsValid(SpawnedBossPortal))
    {
        SpawnedBossPortal->bIsActive = true;
        UE_LOG(LogTemp, Log, TEXT("OnSecretLevelCompleted: Boss portal reactivated."));
    }
}
