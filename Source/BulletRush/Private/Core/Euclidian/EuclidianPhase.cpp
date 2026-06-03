// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Euclidian/EuclidianPhase.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Enemies/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Enemies/EnemyWaveSpawn.h"
void UEuclidianPhase::EnterPhase(AEuclidianGameMode* GameMode)
{
}
void UEuclidianPhase::ExitPhase(AEuclidianGameMode* GameMode)
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

	// ---------------------------
	// Destroy ALL spawners
	// ---------------------------
	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsWithTag(World, "Spawner", Spawners);

	for (AActor* Actor : Spawners)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	// ---------------------------
	// Destroy ALL enemies
	// ---------------------------
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(World, "Enemy", Enemies);

	for (AActor* Actor : Enemies)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}