// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Engine/World.h"

AOrchestratorGameMode::AOrchestratorGameMode()
{
	LevelFacade = nullptr;
}

void AOrchestratorGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LevelFacade = GetWorld()->SpawnActor<AOrchestratorFacade>(AOrchestratorFacade::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (LevelFacade)
		{
			UE_LOG(LogTemp, Warning, TEXT("OrchestratorGameMode: Fachada del Nivel 5 instanciada con éxito."));
		}
	}
}