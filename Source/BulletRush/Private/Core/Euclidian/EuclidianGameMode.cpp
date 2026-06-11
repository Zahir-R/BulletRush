#include "Core/Euclidian/EuclidianGameMode.h"
#include "Enemies/Euclidian/RedTurret.h"
#include "Enemies/Euclidian/RedDrone.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Euclidian/Phase1.h"
#include "Core/Euclidian/PhaseS.h"
#include "Core/Euclidian/Phase2.h"
#include "Engine/World.h"
#include "Map/PortalManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/Euclidian/Strategies/RedTurretObjective.h"
#include "Core/BulletRushGameInstance.h"
#include "Player/PlayingPlayer.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"

AEuclidianGameMode::AEuclidianGameMode()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> CombatFinder(TEXT("SoundWave'/Game/Audio/1-_Brave_reaction.1-_Brave_reaction'"));
	if (CombatFinder.Succeeded()) CombatSong = CombatFinder.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
	if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;
}

void AEuclidianGameMode::BeginPlay()
{
	Super::BeginPlay();

	
    if (GetWorld())
    {
        SkySphere =
            GetWorld()->SpawnActor<ASkySphereWorld>(
                ASkySphereWorld::StaticClass(),
                FTransform(FVector::ZeroVector)
            );
    }

	ChangePhase(
		NewObject<UPhase2>(this)
	);

	if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
	{
		float StartTime = CombatStartOffset;
		if (Music->IsPositionSaved()) StartTime = Music->ConsumeSavedPosition();
		Music->PlaySong(CombatSong, StartTime, 2.0f, true);
	}

	APlayingPlayer* Player = Cast<APlayingPlayer>(
		UGameplayStatics::GetPlayerPawn(this, 0)
	);
	if (Player && Player->HealthComp)
	{
		Player->HealthComp->OnDeath.AddDynamic(
			this,
			&AEuclidianGameMode::OnPlayerDeath
		);
	}
}

void AEuclidianGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(
		UGameplayStatics::GetPlayerPawn(this, 0)
	);
	if (Player && Player->HealthComp)
	{
		Player->HealthComp->OnDeath.RemoveDynamic(
			this,
			&AEuclidianGameMode::OnPlayerDeath
		);
	}
	Super::EndPlay(EndPlayReason);
}

void AEuclidianGameMode::OnPlayerDeath()
{
	if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
		Music->NotifyLevelTravel();

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(
		GetGameInstance()
	);
	if (!GI) return;

	FName MapName = FName(*GetWorld()->GetName());
	int32 VidasRestantes = GI->DecrementarVida(MapName);

	if (VidasRestantes > 0)
	{
		UGameplayStatics::OpenLevel(this, MapName);
	}
	else
	{
		GI->ResetVidas(MapName);
		UGameplayStatics::OpenLevel(this, FName("Map_CupHeadMap"));
	}
}

void AEuclidianGameMode::OnDroneDestroyed(
	AEnemyBase * DeadEnemy)
{
	DeadDroneCount++;

	if (DeadDroneCount % 6 == 0)
	{
		SpawnRedDrone();
	}
}

void AEuclidianGameMode::SpawnRedDrone()
{
	FVector SpawnLocation(
		FMath::RandRange(-1500.f, 1500.f),
		FMath::RandRange(-1500.f, 1500.f),
		100.f
	);

	GetWorld()->SpawnActor<ARedDrone>(
		ARedDrone::StaticClass(),
		SpawnLocation,
		FRotator::ZeroRotator
	);

	RefreshDroneList();
}
void AEuclidianGameMode::RefreshDroneList()
{

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ADrone::StaticClass(),
		FoundDrones
	);

	for (AActor* Actor : FoundDrones)
	{
		if (ADrone* Drone = Cast<ADrone>(Actor))
		{
			Drone->OnEnemyDeath.RemoveDynamic(
				this,
				&AEuclidianGameMode::OnDroneDestroyed
			);

			Drone->OnEnemyDeath.AddDynamic(
				this,
				&AEuclidianGameMode::OnDroneDestroyed
			);

		}
	}
}
void AEuclidianGameMode::OnObservedEnemyDeath(AEnemyBase* Enemy)
{
	IPhaseObjectiveInterface* Objective =
		Cast<IPhaseObjectiveInterface>(CurrentObjective);

	if (!Objective)
	{
		return;
	}

	Objective->OnEnemyKilled(Enemy);

	if (!Objective->IsCompleted())
	{
		return;
	}

	// Phase 1 finished -> go to Phase 2
	if (Cast<UPhase1>(CurrentPhase))
	{
		ChangePhase(
			NewObject<UPhase2>(this)
		);
		return;
	}

	// Phase 2 finished -> spawn CupHead portal
	if (Cast<UPhase2>(CurrentPhase))
	{
		if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
			Music->PlaySong(AmbientSong, 0.0f, 2.0f);

		APortalManager* PortalManager =
			GetWorld()->SpawnActor<APortalManager>();

		if (PortalManager)
		{
			PortalManager->VolverCupHead(
				Enemy->GetActorLocation()
			);
		}
	}
}
void AEuclidianGameMode::SetObjective(
	UObject* NewObjective)
{
	CurrentObjective = NewObjective;

	if (IPhaseObjectiveInterface* Objective =
		Cast<IPhaseObjectiveInterface>(CurrentObjective))
	{
		Objective->Initialize(this);
	}
}
void AEuclidianGameMode::ChangePhase(
	UEuclidianPhase* NewPhase)
{
	if (CurrentPhase)
	{
		CurrentPhase->ExitPhase(this);
	}

	CurrentPhase = NewPhase;

	if (CurrentPhase)
	{
		CurrentPhase->EnterPhase(this);
	}

	RefreshDroneList();
}
void AEuclidianGameMode::EnableRedTurretVulnerability(
	float Duration)
{
	bRedTurretsVulnerable = true;

	GetWorldTimerManager().ClearTimer(
		RedTurretVulnerabilityTimer
	);

	GetWorldTimerManager().SetTimer(
		RedTurretVulnerabilityTimer,
		this,
		&AEuclidianGameMode::DisableRedTurretVulnerability,
		Duration,
		false
	);
}

void AEuclidianGameMode::DisableRedTurretVulnerability()
{
	bRedTurretsVulnerable = false;
}