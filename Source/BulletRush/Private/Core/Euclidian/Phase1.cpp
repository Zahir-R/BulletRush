#include "Core/Euclidian/Phase1.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Core/Euclidian/Strategies/RedTurretObjective.h"

#include "Enemies/EnemyWaveSpawn.h"
#include "Enemies/Common/Drone.h"
#include "Enemies/Euclidian/RedTurret.h"


void UPhase1::EnterPhase(AEuclidianGameMode* GameMode)
{
	if (!GameMode)
	{
		return;
	}

	UWorld* World = GameMode->GetWorld();

	if (!World)
	{
		return;
	}

	//--------------------------------------------------
	// 4 Drone Spawners
	//--------------------------------------------------

	TArray<FVector> SpawnerPositions =
	{
		FVector(1000.f,  1000.f, 100.f),
		FVector(1000.f, -1000.f, 100.f),
		FVector(-1000.f,  1000.f, 100.f),
		FVector(-1000.f, -1000.f, 100.f)
	};

	for (const FVector& Pos : SpawnerPositions)
	{
		AEnemyWaveSpawn* Spawner =
			World->SpawnActor<AEnemyWaveSpawn>(
				AEnemyWaveSpawn::StaticClass(),
				Pos,
				FRotator::ZeroRotator
			);

		if (Spawner)
		{
			Spawner->EnemyClass =
				ADrone::StaticClass();
			Spawner->SpawnWave();
		}
	}

	//--------------------------------------------------
	// 3 Red Turrets
	//--------------------------------------------------

	World->SpawnActor<ARedTurret>(
		ARedTurret::StaticClass(),
		FVector(0.f, 1200.f, 100.f),
		FRotator::ZeroRotator
	);

	World->SpawnActor<ARedTurret>(
		ARedTurret::StaticClass(),
		FVector(-1200.f, 0.f, 100.f),
		FRotator::ZeroRotator
	);

	World->SpawnActor<ARedTurret>(
		ARedTurret::StaticClass(),
		FVector(1200.f, 0.f, 100.f),
		FRotator::ZeroRotator
	);
	// Objective: kill 3 red turrets
	GameMode->SetObjective(
		NewObject<URedTurretObjective>(GameMode)
	);
}