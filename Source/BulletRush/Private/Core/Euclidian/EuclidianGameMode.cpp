#include "Core/Euclidian/EuclidianGameMode.h"
#include "Enemies/Euclidian/RedTurret.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Euclidian/Phase1.h"
#include "Core/Euclidian/PhaseS.h"
#include "Core/Euclidian/Phase2.h"
#include "Core/Euclidian/Strategies/RedTurretObjective.h"

void AEuclidianGameMode::BeginPlay()
{	
	CurrentPhase = NewObject<UPhase1>(this);

	if (CurrentPhase)
	{
		CurrentPhase->EnterPhase(this);
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
			NewObject<UPhaseS>(this)
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
}
void AEuclidianGameMode::RegisterDroneDeath()
{
	DeadDroneCount++;

	UE_LOG(LogTemp, Warning, TEXT("Drones dead: %d / 8"), DeadDroneCount);
}