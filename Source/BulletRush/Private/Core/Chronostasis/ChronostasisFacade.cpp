#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Core/Chronostasis/GameModeChronostasis.h"
#include "Core/Chronostasis/ChronostasisWaveManager.h"
#include "Core/Chronostasis/ChronostasisSlowSystem.h"
#include "Core/Chronostasis/ChronostasisBossManager.h"
#include "Core/Chronostasis/ChronostasisSecretManager.h"
#include "Core/BulletRushGameModeBase.h"
#include "Enemies/EnemyBase.h"
#include "Components/HealthComponent.h"
#include "Core/BulletRushHUD.h"
#include "Core/Requirements/RequirementManager.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABulletRushHUD* AChronostasisFacade::GetHUD() const
{
    APlayerController* PC = OwningGameMode ? UGameplayStatics::GetPlayerController(OwningGameMode->GetWorld(), 0) : nullptr;
    return PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
}

AChronostasisFacade::AChronostasisFacade()
{
    PrimaryActorTick.bCanEverTick = false;
}

AChronostasisFacade::~AChronostasisFacade()
{
}

void AChronostasisFacade::BeginPlay()
{
    Super::BeginPlay();

    WaveManager = NewObject<UChronostasisWaveManager>(this);
    SlowSystem = NewObject<UChronostasisSlowSystem>(this);
    BossManager = NewObject<UChronostasisBossManager>(this);
    SecretManager = NewObject<UChronostasisSecretManager>(this);

    WaveManager->Initialize(this, Waves);
    SlowSystem->Initialize(this);
    BossManager->Initialize(this, SerXBossClass, BossArenaSpawnLocation);
    SecretManager->Initialize(this);

    WaveManager->OnAllWavesCompleted.AddUObject(this, &AChronostasisFacade::OnAllWavesComplete);
    WaveManager->OnWaveEnemyKilled.AddUObject(this, &AChronostasisFacade::OnEnemyKilled);
    SlowSystem->OnSlowTriggered.AddUObject(this, &AChronostasisFacade::NotifySubscribers);
    BossManager->OnBossDefeated.AddUObject(this, &AChronostasisFacade::NotifySubscribers);
    SecretManager->OnSecretTimeUp.AddUObject(this, &AChronostasisFacade::OnSecretTimeUp);

    OwningGameMode = Cast<ABulletRushGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void AChronostasisFacade::StartGame()
{
    SlowSystem->Start();
    WaveManager->StartGame();

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
}

void AChronostasisFacade::OnEnemyKilled(AEnemyBase* Enemy)
{
    SlowSystem->ResetOnKill();
}

void AChronostasisFacade::OnAllWavesComplete()
{
    UWorld* World = GetWorld();
    if (!World) return;

    World->GetTimerManager().ClearAllTimersForObject(this);
    SecretManager->ClearAllTimers();

    if (SecretManager->IsActive())
    {
        SecretManager->Reset();

        UProjectilesSubsystem* ProjSys = World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
        if (ProjSys) ProjSys->SetSecretLevel(false);

        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM) GM->OnSecretLevelCompleted();
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
            bSecretUnlocked = (SlowSystem->GetTriggerCount() >= 3) && !bPlayerTookDamage;
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

void AChronostasisFacade::OnSecretTimeUp()
{
    if (OwningGameMode)
    {
        AGameModeChronostasis* GM = Cast<AGameModeChronostasis>(OwningGameMode);
        if (GM) GM->OnSecretLevelCompleted();
    }
}

int32 AChronostasisFacade::GetSlowTriggerCount() const
{
    return SlowSystem ? SlowSystem->GetTriggerCount() : 0;
}

void AChronostasisFacade::SetRequirementManager(URequirementManager* Manager)
{
    RequirementManagerRef = Manager;
}

void AChronostasisFacade::StartSecretWaves(const TArray<FWaveConfig>& NewWaves)
{
    WaveManager->StartSecretWaves(NewWaves);
    SecretManager->Start();

    UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
    if (ProjSys) ProjSys->SetSecretLevel(true);

    ABulletRushHUD* HUD = GetHUD();
    if (HUD)
    {
        HUD->ShowMessage("SECRET LEVEL", 3.f);
    }
}

bool AChronostasisFacade::AreAllWavesComplete() const
{
    return WaveManager && WaveManager->AreAllWavesComplete();
}

void AChronostasisFacade::StartBossFight()
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearAllTimersForObject(this);
    }
    SecretManager->ClearAllTimers();
    SlowSystem->Stop();

    BossManager->StartBossFight();
}

void AChronostasisFacade::OnBossKilled(AEnemyBase* Boss)
{
    BossManager->OnBossKilled(Boss);
}

void AChronostasisFacade::OnBossPortalTriggered()
{
    StartBossFight();
}

void AChronostasisFacade::OnPlayerHealthChanged(float NewHealth)
{
    if (!PlayerHealthComp.IsValid()) return;
    if (NewHealth < PlayerHealthComp->MaxHealth)
    {
        bPlayerTookDamage = true;
        PlayerHealthComp->OnHealthChanged.RemoveDynamic(this, &AChronostasisFacade::OnPlayerHealthChanged);
    }
}
