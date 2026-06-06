// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/Euclidian/Phase2.h"
#include "Enemies/Euclidian/Tesseriel.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Engine/World.h"

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
		FVector(0.f, 0.f, 100.f),
		FRotator::ZeroRotator
	);
}