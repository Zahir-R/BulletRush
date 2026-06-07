#include "Core/Chronostasis/ChronostasisNormalFacade.h"
#include "Core/Chronostasis/ChronostasisWaveManager.h"
#include "Core/Chronostasis/ChronostasisSlowSystem.h"
#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Core/Requirements/RequirementManager.h"
#include "Core/Requirements/NoDamageRequirement.h"
#include "Core/Requirements/TimeStopRequirement.h"
#include "Core/BulletRushGameInstance.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Map/PortalTrigger.h"
#include "Components/HealthComponent.h"
#include "Player/PlayingPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AChronostasisNormalFacade::AChronostasisNormalFacade()
{
	PrimaryActorTick.bCanEverTick = false;

	FWaveConfig NW1; NW1.DroneCount = 3; NW1.MassCount = 2;
	FWaveConfig NW2; NW2.DroneCount = 3; NW2.MassCount = 1; NW2.ExpansiveCount = 1;
	FWaveConfig NW3; NW3.DroneCount = 3; NW3.ExpansiveCount = 2;
	Waves = { NW1, NW2, NW3 };
}

void AChronostasisNormalFacade::BeginPlay()
{
	Super::BeginPlay();

	WaveManager = NewObject<UChronostasisWaveManager>(this);
	SlowSystem = NewObject<UChronostasisSlowSystem>(this);

	WaveManager->Initialize(this, Waves);
	SlowSystem->Initialize(this);

	WaveManager->OnAllWavesCompleted.AddUObject(this, &AChronostasisNormalFacade::OnAllWavesComplete);
	WaveManager->OnWaveEnemyKilled.AddUObject(this, &AChronostasisNormalFacade::OnEnemyKilled);
	SlowSystem->OnSlowTriggered.AddUObject(this, &AChronostasisNormalFacade::NotifySubscribers);

	PortalBossTrigger = GetWorld()->SpawnActor<APortalTrigger>(
		APortalTrigger::StaticClass(),
		BossPortalLocation, FRotator::ZeroRotator);
	if (PortalBossTrigger)
	{
		PortalBossTrigger->bIsActive = false;
		PortalBossTrigger->SetActorHiddenInGame(true);
		PortalBossTrigger->SetActorEnableCollision(false);
		PortalBossTrigger->OnPortalTriggered.AddUObject(this, &AChronostasisNormalFacade::OnBossPortalTriggered);
	}

	PortalSecretTrigger = GetWorld()->SpawnActor<APortalTrigger>(
		APortalTrigger::StaticClass(),
		SecretPortalLocation, FRotator::ZeroRotator);
	if (PortalSecretTrigger)
	{
		PortalSecretTrigger->bIsActive = false;
		PortalSecretTrigger->SetActorHiddenInGame(true);
		PortalSecretTrigger->SetActorEnableCollision(false);
		PortalSecretTrigger->OnPortalTriggered.AddUObject(this, &AChronostasisNormalFacade::OnSecretPortalTriggered);
	}
}

void AChronostasisNormalFacade::StartLevel()
{
	WaveManager->SetWaves(Waves);
	SlowSystem->Start();
	WaveManager->StartGame();

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		APawn* Pawn = PC->GetPawn();
		URequirementManager* ReqMgr = nullptr;
		if (Pawn)
		{
			ReqMgr = Pawn->FindComponentByClass<URequirementManager>();
		}
		if (!ReqMgr)
		{
			ReqMgr = NewObject<URequirementManager>(PC);
			ReqMgr->RegisterComponent();
		}

		UNoDamageRequirement* NoDamageReq = NewObject<UNoDamageRequirement>(ReqMgr);
		UTimeStopRequirement* TimeStopReq = NewObject<UTimeStopRequirement>(ReqMgr);
		TimeStopReq->RequiredStops = 3;
		//ReqMgr->SecretRequirements.Add(NoDamageReq);
		ReqMgr->SecretRequirements.Add(TimeStopReq);

		ReqMgr->InitializeRequirements(PC);
		RequirementManagerRef = ReqMgr;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		UHealthComponent* HealthComp = PlayerPawn->FindComponentByClass<UHealthComponent>();
		if (HealthComp)
		{
			PlayerHealthComp = HealthComp;
			HealthComp->OnHealthChanged.AddDynamic(this, &AChronostasisNormalFacade::OnPlayerHealthChanged);
		}
	}
}

void AChronostasisNormalFacade::OnEnemyKilled(AEnemyBase* Enemy)
{
	SlowSystem->ResetOnKill();
}

void AChronostasisNormalFacade::OnAllWavesComplete()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearAllTimersForObject(this);
	SlowSystem->Stop();

	APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(World, 0));
	if (Player && Player->BuffComp)
	{
		Player->BuffComp->RemoveAllDecorators();
	}

	bool bSecretUnlocked = RequirementManagerRef.IsValid() && RequirementManagerRef->AreSecretRequirementsMet();

	ActivateBossPortal();

	if (bSecretUnlocked)
	{
		ActivateSecretPortal();
	}
}

void AChronostasisNormalFacade::ActivateBossPortal()
{
	if (bPortalsActivated || !PortalBossTrigger) return;
	PortalBossTrigger->bIsActive = true;
	PortalBossTrigger->SetActorHiddenInGame(false);
	PortalBossTrigger->SetActorEnableCollision(true);
}

void AChronostasisNormalFacade::ActivateSecretPortal()
{
	if (!PortalSecretTrigger) return;
	PortalSecretTrigger->bIsActive = true;
	PortalSecretTrigger->SetActorHiddenInGame(false);
	PortalSecretTrigger->SetActorEnableCollision(true);
}

void AChronostasisNormalFacade::OnBossPortalTriggered()
{
	TravelToMap(FName("Map_03Boss"), ELevelState::Boss);
}

void AChronostasisNormalFacade::OnSecretPortalTriggered()
{
	TravelToMap(FName("Map_03Boss"), ELevelState::Secret);
}

void AChronostasisNormalFacade::TravelToMap(FName MapName, ELevelState NextState)
{
	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (!GI) return;

	GI->ChronostasisState = NextState;
	bPortalsActivated = true;

	UGameplayStatics::OpenLevel(this, MapName);
}

void AChronostasisNormalFacade::OnPlayerHealthChanged(float NewHealth)
{
	if (!PlayerHealthComp.IsValid()) return;
	if (NewHealth < PlayerHealthComp->MaxHealth)
	{
		bPlayerTookDamage = true;
		PlayerHealthComp->OnHealthChanged.RemoveDynamic(this, &AChronostasisNormalFacade::OnPlayerHealthChanged);
	}
}
