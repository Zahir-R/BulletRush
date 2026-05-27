// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultKeeper/core/MechaEnemyFactory.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/enemies/VaultKeeper.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Engine/World.h"

// Sets default values
AMechaEnemyFactory::AMechaEnemyFactory()
{
	PrimaryActorTick.bCanEverTick = false;

	
	DronMechaClass = ADronMecha::StaticClass();
	BatteryActorClass = ABatteryActor::StaticClass();
	VaultKeeperClass = AVaultKeeper::StaticClass();
}

// Called when the game starts or when spawned
void AMechaEnemyFactory::BeginPlay()
{
	Super::BeginPlay();
}

AActor* AMechaEnemyFactory::CreateEnemy(EMechaEnemyType EnemyType, FVector Location, FRotator Rotation)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[MechaFactory] Error: No se pudo obtener el World."));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this; // Indicamos que la fábrica es la dueña inicial del actor creado

	AActor* NewEnemy = nullptr;
	TSubclassOf<AActor> ClassToSpawn = nullptr;

	// Selección del producto según el patrón Factory Method
	switch (EnemyType)
	{
	case EMechaEnemyType::DroneMecha:
		ClassToSpawn = DronMechaClass;
		break;

	case EMechaEnemyType::BatteryActor:
		ClassToSpawn = BatteryActorClass;
		break;

	case EMechaEnemyType::VaultKeeper:
		ClassToSpawn = VaultKeeperClass;
		break;

	default:
		break;
	}

	if (ClassToSpawn)
	{
		NewEnemy = World->SpawnActor<AActor>(ClassToSpawn, Location, Rotation, SpawnParams);

		if (NewEnemy)
		{
			UE_LOG(LogTemp, Display, TEXT("[MechaFactory] Éxito: Se fabricó el actor %s."), *NewEnemy->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[MechaFactory] Error: La clase asociada al tipo de enemigo es nula."));
	}

	return NewEnemy;
}