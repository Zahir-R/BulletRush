#include "Core/Chronostasis/ChronostasisSecretFacade.h"
#include "Core/Chronostasis/ChronostasisWaveManager.h"
#include "Core/Chronostasis/ChronostasisSlowSystem.h"
#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/BulletRushHUD.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Enemies/EnemyBase.h"
#include "Components/BuffComponent.h"
#include "Map/PortalTrigger.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Player/PlayingPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AChronostasisSecretFacade::AChronostasisSecretFacade()
{
	PrimaryActorTick.bCanEverTick = false;

	FWaveConfig SW1; SW1.DroneCount = 5;
	FWaveConfig SW2; SW2.DroneCount = 3; SW2.ExpansiveCount = 2;
	FWaveConfig SW3; SW3.DroneCount = 2; SW3.MassCount = 3;
	Waves = { SW1, SW2, SW3 };
}

void AChronostasisSecretFacade::BeginPlay()
{
	Super::BeginPlay();

	WaveManager = NewObject<UChronostasisWaveManager>(this);
	SlowSystem = NewObject<UChronostasisSlowSystem>(this);

	WaveManager->Initialize(this, Waves);
	SlowSystem->Initialize(this);

	WaveManager->OnAllWavesCompleted.AddUObject(this, &AChronostasisSecretFacade::OnAllWavesComplete);
	WaveManager->OnWaveEnemyKilled.AddUObject(this, &AChronostasisSecretFacade::OnEnemyKilled);

	PortalBossTrigger = GetWorld()->SpawnActor<APortalTrigger>(
		APortalTrigger::StaticClass(),
		PortalLocation, FRotator::ZeroRotator);
	if (PortalBossTrigger)
	{
		PortalBossTrigger->bIsActive = false;
		PortalBossTrigger->SetActorHiddenInGame(true);
		PortalBossTrigger->SetActorEnableCollision(false);
		PortalBossTrigger->OnPortalTriggered.AddUObject(this, &AChronostasisSecretFacade::OnPortalToBossTriggered);
	}
}

void AChronostasisSecretFacade::StartLevel()
{
	WaveManager->StartSecretWaves(Waves);
	SlowSystem->Start();

	TimeRemaining = TimeLimit;
	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimerHandle, this, &AChronostasisSecretFacade::TickTimer, 1.0f, true);

	UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
	if (ProjSys) ProjSys->SetSecretLevel(true);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
	if (HUD)
	{
		HUD->ShowMessage("SECRET LEVEL", 3.0f);
	}
}

void AChronostasisSecretFacade::OnEnemyKilled(AEnemyBase* Enemy)
{
}

void AChronostasisSecretFacade::OnAllWavesComplete()
{
	if (bLevelComplete) return;
	bLevelComplete = true;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}

	UProjectilesSubsystem* ProjSys = World ? World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>() : nullptr;
	if (ProjSys) ProjSys->SetSecretLevel(false);

	SlowSystem->Stop();

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->PowerUpCooldownMultiplier = 0.5f;
	}

	APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
	ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
	if (HUD)
	{
		HUD->ShowMessage("REWARD: PowerUp cooldown halved!", 5.0f);
	}

	APlayingPlayer* Player = PC ? Cast<APlayingPlayer>(PC->GetPawn()) : nullptr;
	if (Player && Player->BuffComp)
	{
		Player->BuffComp->RemoveAllDecorators();
	}

	if (PortalBossTrigger)
	{
		PortalBossTrigger->bIsActive = true;
		PortalBossTrigger->SetActorHiddenInGame(false);
		PortalBossTrigger->SetActorEnableCollision(true);
	}
}

void AChronostasisSecretFacade::OnTimeUp()
{
	if (bLevelComplete) return;
	bLevelComplete = true;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}

	UProjectilesSubsystem* ProjSys = World ? World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>() : nullptr;
	if (ProjSys) ProjSys->SetSecretLevel(false);

	SlowSystem->Stop();
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Found);
	for (AActor* A : Found)
	{
		A->Destroy();
	}


	UE_LOG(LogTemp, Warning, TEXT("[ChronostasisSecretFacade] Secret level failed (time expired)."));

	APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
	ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
	if (HUD)
	{
		HUD->ShowMessage("TIME'S UP! Teleporting...", 3.0f);
	}

	APlayingPlayer* Player = PC ? Cast<APlayingPlayer>(PC->GetPawn()) : nullptr;
	if (Player && Player->BuffComp)
	{
		Player->BuffComp->RemoveAllDecorators();
	}

	if (PortalBossTrigger)
	{
		PortalBossTrigger->bIsActive = true;
		PortalBossTrigger->SetActorHiddenInGame(false);
		PortalBossTrigger->SetActorEnableCollision(true);
	}

	if (World)
	{
		World->GetTimerManager().SetTimer(AutoTeleportTimer, this, &AChronostasisSecretFacade::OnAutoTeleportToBoss, 3.0f, false);
	}
}

void AChronostasisSecretFacade::TickTimer()
{
	TimeRemaining -= 1.0f;

	UWorld* World = GetWorld();
	APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
	ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
	if (HUD)
	{
		HUD->SetCountdown(TimeRemaining);
	}

	if (TimeRemaining <= 0.0f)
	{
		if (World)
		{
			World->GetTimerManager().ClearTimer(CountdownTimerHandle);
		}
		OnTimeUp();
	}
}

void AChronostasisSecretFacade::OnPortalToBossTriggered()
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI) return;

	GI->ChronostasisState = ELevelState::Boss;
	UGameplayStatics::OpenLevel(this, FName("Map_03Boss"));
}

void AChronostasisSecretFacade::OnAutoTeleportToBoss()
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI) return;

	GI->ChronostasisState = ELevelState::Boss;
	UGameplayStatics::OpenLevel(this, FName("Map_03Boss"));
}
