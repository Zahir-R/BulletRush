// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Enemies/Orchestrator/Orchestrator.h"
#include "Enemies/EnemyBase.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AOrchestratorFacade::AOrchestratorFacade()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsPlayerDetected = false;
	bSecretPuzzleSolved = false;
	bBossFightStarted = false;

	GeneratorsDestroyed = 0;
	GuardiansDefeated = 0;
	TotalGuardiansToSpawn = 3;
}

void AOrchestratorFacade::BeginPlay()
{
	Super::BeginPlay();
}

void AOrchestratorFacade::HandlePlayerDetected(FVector DetectionLocation)
{
	if (bIsPlayerDetected || bBossFightStarted) return; // Evitar spam

	bIsPlayerDetected = true;
	UE_LOG(LogTemp, Warning, TEXT("Fachada: ¡Jugador detectado! Sigilo roto. Invocando refuerzos..."));

	// Generamos los refuerzos que dicta el GDD alrededor del punto de detección
	SpawnZoneAReinforcements(DetectionLocation);
}

void AOrchestratorFacade::ReportGeneratorDestroyed()
{
	GeneratorsDestroyed++;
	UE_LOG(LogTemp, Warning, TEXT("Fachada: Generador destruido (%d/3)."), GeneratorsDestroyed);

	if (GeneratorsDestroyed == 1)
	{
		// Inicia el cronómetro de 5 segundos para el puzzle
		GetWorld()->GetTimerManager().SetTimer(PuzzleTimerHandle, this, &AOrchestratorFacade::FailSecretPuzzle, 5.0f, false);
	}
	else if (GeneratorsDestroyed >= 3)
	{
		// Puzzle resuelto a tiempo
		GetWorld()->GetTimerManager().ClearTimer(PuzzleTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Fachada: Puzzle a tiempo. Spawneando Guardianes Secretos."));
		SpawnSecretGuardians();
	}
}

void AOrchestratorFacade::FailSecretPuzzle()
{
	UE_LOG(LogTemp, Error, TEXT("Fachada: Tiempo agotado. Puzzle 5-1-S fallado."));
	GeneratorsDestroyed = 0;
	// Aquí podrías destruir los generadores restantes o bloquear la habitación
}

void AOrchestratorFacade::ReportGuardianDefeated()
{
	GuardiansDefeated++;

	if (GuardiansDefeated >= TotalGuardiansToSpawn)
	{
		bSecretPuzzleSolved = true;
		UE_LOG(LogTemp, Warning, TEXT("Fachada: ¡Guardianes derrotados! Nivel 5-1-S completado. Jefe perderá Fase 1."));
	}
}

void AOrchestratorFacade::PrepareBossArena(FTransform BossSpawnTransform)
{
	if (bBossFightStarted) return;
	bBossFightStarted = true;

	UE_LOG(LogTemp, Warning, TEXT("Fachada: Iniciando Combate de Jefe."));

	if (GetWorld())
	{
		AOrchestrator* TheBoss = GetWorld()->SpawnActor<AOrchestrator>(AOrchestrator::StaticClass(), BossSpawnTransform);
		if (TheBoss)
		{
			// Aplicamos la resolución del puzzle 5-1-S al jefe para saltar fases
			TheBoss->bSecretLevelCleared = bSecretPuzzleSolved;
		}
	}
}

void AOrchestratorFacade::SpawnZoneAReinforcements(FVector SpawnOrigin)
{
	if (!ReinforcementClass || !GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn de 3 enemigos en un triángulo alrededor del jugador
	for (int i = 0; i < 3; i++)
	{
		FVector Offset = FVector(FMath::Cos(i * 120.0f) * 400.0f, FMath::Sin(i * 120.0f) * 400.0f, 0.0f);
		GetWorld()->SpawnActor<AEnemyBase>(ReinforcementClass, SpawnOrigin + Offset, FRotator::ZeroRotator, SpawnParams);
	}
}

void AOrchestratorFacade::SpawnSecretGuardians()
{
	if (!SecretGuardianClass || !GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int i = 0; i < TotalGuardiansToSpawn; i++)
	{
		// Aparecen frente al jugador en la sala de energía
		FVector SpawnLoc = GetActorLocation() + FVector(200.0f * i, 300.0f, 0.0f);
		AEnemyBase* Guardian = GetWorld()->SpawnActor<AEnemyBase>(SecretGuardianClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

		if (Guardian)
		{
			// Nos suscribimos a su muerte para llevar la cuenta
			//Guardian->OnEnemyDeath.AddDynamic(this, &AOrchestratorFacade::ReportGuardianDefeated);
		}
	}
}