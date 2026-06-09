#include "Core/Euclidian/Phase2.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Core/Euclidian/Strategies/BossObjective.h"

#include "Enemies/Euclidian/Tesseriel.h"
#include "Enemies/EnemyWaveSpawn.h"

void UPhase2::EnterPhase(
	AEuclidianGameMode* GameMode)
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

	World->SpawnActor<ATesseriel>(
		ATesseriel::StaticClass(),
		FVector(0.f, 0.f, 600.f),
		FRotator::ZeroRotator
	);


	// 4 Drone Spawners

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


	GameMode->SetObjective(
		NewObject<UBossObjective>(
			GameMode
		)
	);
}