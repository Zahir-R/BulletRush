// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "OrchestratorFacade.generated.h"

class AOrchestratorGameMode;
class AEnemyBase;
class AOrchestrator;
class ABossArenaTrigger;

UCLASS()
class BULLETRUSH_API AOrchestratorFacade : public AActor
{
	GENERATED_BODY()
	
public:	
	AOrchestratorFacade();

	virtual void Tick(float DeltaTime) override;

	// --- ZONA A (Nivel 5-1: Infiltración) ---
	UFUNCTION(BlueprintCallable, Category = "Facade|Zone A")
	void HandlePlayerDetected(FVector DetectionLocation);

	// --- ZONA B (Nivel 5-1-S: Fuente de Poder) ---
	UFUNCTION(BlueprintCallable, Category = "Facade|Zone B")
	void ReportGeneratorDestroyed();

	UFUNCTION(BlueprintCallable, Category = "Facade|Zone B")
	void ReportGuardianDefeated();

	// --- ZONA C (Nivel 5-2: Combate de Jefe) ---
	UFUNCTION(BlueprintCallable, Category = "Facade|Zone C")
	void PrepareBossArena(FTransform BossSpawnTransform);

	// Configuración de clases a spawnear (Configurables en un Blueprint hijo de la fachada o por código)
	UPROPERTY(EditAnywhere, Category = "Facade|Classes")
	TSubclassOf<AEnemyBase> ReinforcementClass;

	UPROPERTY(EditAnywhere, Category = "Facade|Classes")
	TSubclassOf<AEnemyBase> SecretGuardianClass;

	bool bSecretPuzzleSolved;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* LevelAudioComp;

	UPROPERTY(EditDefaultsOnly, Category = "Audio|Tracks")
	USoundBase* ChillMusic;

	AOrchestrator* Boss;

	UPROPERTY(EditDefaultsOnly, Category = "Facade|Triggers")
	ABossArenaTrigger* TriggerRef;

protected:
	virtual void BeginPlay() override;

private:
	// Variables de estado
	bool bIsPlayerDetected;
	bool bBossFightStarted;

	// Variables del Puzzle 5-1-S
	int32 GeneratorsDestroyed;
	int32 GuardiansDefeated;
	int32 TotalGuardiansToSpawn;
	FTimerHandle PuzzleTimerHandle;
	float PuzzleTimeRemaining;
	bool bPuzzleActive;

	// Funciones internas ocultas
	void FailSecretPuzzle();
	void SpawnZoneAReinforcements(FVector SpawnOrigin);
	void SpawnSecretGuardians();
};
