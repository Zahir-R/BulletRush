// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Engine/World.h"
#include "TimerManager.h"

AVaultKeeperFacade::AVaultKeeperFacade()
{
	PrimaryActorTick.bCanEverTick = false;
	EnemyFactory = nullptr;
	FactoryClass = AMechaEnemyFactory::StaticClass();
	CurrentWaveIndex = 0;
	TimeRemaining = 180.0f; // 3 minutos de supervivencia
}

void AVaultKeeperFacade::BeginPlay()
{
	Super::BeginPlay();
	InitializeLevel();
}

void AVaultKeeperFacade::InitializeLevel()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// Spawneamos la factoría como un Actor 
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	EnemyFactory = World->SpawnActor<AMechaEnemyFactory>(FactoryClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (EnemyFactory)
	{
		UE_LOG(LogTemp, Display, TEXT("[Level2Facade] Infraestructura lista: Se instanció la Fábrica de Enemigos."));
	}
}

void AVaultKeeperFacade::StartLevelProgression()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UE_LOG(LogTemp, Display, TEXT("[VaultKeeperFacade] ¡Comienza la supervivencia de 3 minutos!"));

	World->GetTimerManager().SetTimer(GlobalLevelTimer, this, &AVaultKeeperFacade::UpdateLevelClock, 1.0f, true);

	SpawnNextWave();
}

void AVaultKeeperFacade::UpdateLevelClock()
{
	TimeRemaining -= 1.0f;

	if (TimeRemaining <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalLevelTimer);
		GetWorld()->GetTimerManager().ClearTimer(WaveSpawnTimer);

		SpawnBossBattle();
	}
	else if (FMath::Fmod(TimeRemaining, 30.0f) == 0.0f)
	{
		SpawnNextWave();
	}
}

void AVaultKeeperFacade::SpawnNextWave()
{
	if (!EnemyFactory) return;

	CurrentWaveIndex++;
	UE_LOG(LogTemp, Warning, TEXT("[VaultKeeperFacade] Spawneando Oleada Nro: %d"), CurrentWaveIndex);

	FVector DroneLocation = GetActorLocation() + FVector(FMath::FRandRange(-500.f, 500.f), FMath::FRandRange(-500.f, 500.f), 100.f);
	FVector BatteryLocation = DroneLocation + FVector(200.f, 200.f, 0.f); 

	AActor* RawDrone = EnemyFactory->CreateEnemy(EMechaEnemyType::DroneMecha, DroneLocation, FRotator::ZeroRotator);
	AActor* RawBattery = EnemyFactory->CreateEnemy(EMechaEnemyType::BatteryActor, BatteryLocation, FRotator::ZeroRotator);

	ADronMecha* Drone = Cast<ADronMecha>(RawDrone);
	ABatteryActor* Battery = Cast<ABatteryActor>(RawBattery);

	if (Drone && Battery)
	{
		// Vinculamos la batería al dron. Esto añade internamente al dron 
		// en el TArray de 'Suscribers' y amarra el patrón Observer de golpe.
		Battery->LinkDrone(Drone);
		UE_LOG(LogTemp, Display, TEXT("[Level2Facade] Patrón Observer conectado exitosamente en la oleada."));
	}
}

void AVaultKeeperFacade::SpawnBossBattle()
{
	if (!EnemyFactory) return;

	UE_LOG(LogTemp, Error, TEXT("[VaultKeeperFacade] ¡TIEMPO COMPLETADO! Spawneando al Jefe de la Bóveda: VaultKeeper."));
	FVector BossLocation = GetActorLocation() + FVector(0.f, 1000.f, 200.f); // Posición central elevada

	// La factoría se encarga de dar vida al jefe final
	EnemyFactory->CreateEnemy(EMechaEnemyType::VaultKeeper, BossLocation, FRotator::ZeroRotator);
}