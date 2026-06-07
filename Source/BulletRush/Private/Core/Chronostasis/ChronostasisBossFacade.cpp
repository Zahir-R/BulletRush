#include "Core/Chronostasis/ChronostasisBossFacade.h"
#include "Core/Chronostasis/GenericEnemyFactory.h"
#include "Core/BulletRushGameInstance.h"
#include "Core/BulletRushHUD.h"
#include "Enemies/Chronostasis/Boss/SerXBoss.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Enemies/EnemyBase.h"
#include "Components/BuffComponent.h"
#include "Map/PortalTrigger.h"
#include "Player/PlayingPlayer.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AChronostasisBossFacade::AChronostasisBossFacade()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
	if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> CombatFinder(TEXT("SoundWave'/Game/Audio/Combat.Combat'"));
	if (CombatFinder.Succeeded()) CombatSong = CombatFinder.Object;
}

void AChronostasisBossFacade::BeginPlay()
{
	Super::BeginPlay();

	PortalHubTrigger = GetWorld()->SpawnActor<APortalTrigger>(
		APortalTrigger::StaticClass(),
		PortalLocation, FRotator::ZeroRotator);
	if (PortalHubTrigger)
	{
		PortalHubTrigger->bIsActive = false;
		PortalHubTrigger->SetActorHiddenInGame(true);
		PortalHubTrigger->SetActorEnableCollision(false);
		PortalHubTrigger->OnPortalTriggered.AddUObject(this, &AChronostasisBossFacade::OnPortalToHubTriggered);
	}
}

void AChronostasisBossFacade::StartLevel()
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

	SpawnBoss();
}

void AChronostasisBossFacade::SpawnBoss()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TSubclassOf<ASerXBoss> BossClass = SerXBossClass ? SerXBossClass : ASerXBoss::StaticClass();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	BossInstance = World->SpawnActor<ASerXBoss>(BossClass, BossSpawnLocation, FRotator::ZeroRotator, Params);

	if (!BossInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[ChronostasisBossFacade] No se pudo spawnear el SerXBoss"));
		return;
	}

	UChronostasisFactoryEnemy* LinkerFactory = CreateGenericFactory<AChronostasisLinker>(this);
	BossInstance->SetLinkerFactory(LinkerFactory);

	BossInstance->AddMinionFactory(CreateGenericFactory<AChronostasisDrone>(this));
	BossInstance->AddMinionFactory(CreateGenericFactory<AChronostasisExpansive>(this));
	BossInstance->AddMinionFactory(CreateGenericFactory<AChronostasisMass>(this));
	BossInstance->AddMinionFactory(CreateGenericFactory<AChronostasisCharger>(this));

	BossInstance->OnEnemyDeath.AddDynamic(this, &AChronostasisBossFacade::OnBossDeath);

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
	if (HUD)
	{
		HUD->ShowMessage(TEXT("BOSS FIGHT"), 3.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("[ChronostasisBossFacade] SerXBoss spawneado"));
}

void AChronostasisBossFacade::OnBossDeath(AEnemyBase* DeadEnemy)
{
	if (bLevelComplete) return;
	bLevelComplete = true;

	if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
		Music->TransitionTo(AmbientSong, 3.0f, 0.5f, 0.0f);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
		ABulletRushHUD* HUD = PC ? Cast<ABulletRushHUD>(PC->GetHUD()) : nullptr;
		if (HUD)
		{
			HUD->ShowMessage(TEXT("VICTORY"), 5.0f);
		}

		UProjectilesSubsystem* ProjSys = World->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
		if (ProjSys) ProjSys->ReturnAllActiveBullets();
	}

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->MarcarMapaCompletado(FName("Map_03Boss"));
		GI->ChronostasisState = ELevelState::Normal;
	}

	APlayingPlayer* Player = Cast<APlayingPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player && Player->BuffComp)
	{
		Player->BuffComp->RemoveAllDecorators();
	}

	if (PortalHubTrigger)
	{
		PortalHubTrigger->bIsActive = true;
		PortalHubTrigger->SetActorHiddenInGame(false);
		PortalHubTrigger->SetActorEnableCollision(true);
	}
}

void AChronostasisBossFacade::OnPortalToHubTriggered()
{
	UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
}
