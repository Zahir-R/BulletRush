#include "Core/Euclidian/EuclidianGameMode.h"
#include "Enemies/Euclidian/RedTurret.h"
#include "Enemies/Euclidian/RedDrone.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Euclidian/Phase1.h"
#include "Core/Euclidian/PhaseS.h"
#include "Core/Euclidian/Phase2.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/Euclidian/Strategies/RedTurretObjective.h"

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
		NewObject<UPhase1>(this)
	);

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ADrone::StaticClass(),
		FoundDrones
	);
	for (AActor* Actor : FoundDrones)
	{
		ADrone* Drone = Cast<ADrone>(Actor);

		if (Drone)
		{
			Drone->OnEnemyDeath.AddDynamic(
				this,
				&AEuclidianGameMode::OnDroneDestroyed
			);
		}
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

	if (Objective->IsCompleted())
	{
		ChangePhase(
			NewObject<UPhase2>(this)
		);
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