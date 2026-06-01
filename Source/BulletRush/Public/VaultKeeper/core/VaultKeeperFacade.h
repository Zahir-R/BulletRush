// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeperFacade.generated.h"

UCLASS()
class BULLETRUSH_API AVaultKeeperFacade : public AActor
{
	GENERATED_BODY()

public:
	AVaultKeeperFacade();

protected:
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Level2|Facade")
	void InitializeLevel();

	
	UFUNCTION(BlueprintCallable, Category = "Level2|Facade")
	void StartLevelProgression();

private:
	
	UPROPERTY()
	AMechaEnemyFactory* EnemyFactory;

	UPROPERTY(EditDefaultsOnly, Category = "Level2|Setup")
	TSubclassOf<AMechaEnemyFactory> FactoryClass;

	FTimerHandle GlobalLevelTimer;
	FTimerHandle WaveSpawnTimer;

	int32 CurrentWaveIndex;
	float TimeRemaining;

	void UpdateLevelClock();
	void SpawnNextWave();
	void SpawnBossBattle();
};