// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/PortalManager.h"
#include "Map/LevelPortal.h"
#include "Core/BulletRushGameModeBase.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "Engine/World.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalesDataBase.Add(FPortalData("Map_01Boss", FVector(300.0f, 300.0f, 50.0f)));
	PortalesDataBase.Add(FPortalData("Map_02Boss", FVector(-300.0f,300.0f,50.0f)));
	PortalesDataBase.Add(FPortalData("Map_03Boss", FVector(300.0f,-300.0f,50.0f)));
	PortalesDataBase.Add(FPortalData("Map_04Boss", FVector(-200.0f,-400.0f,50.0f)));
	PortalesDataBase.Add(FPortalData("Map_05Boss", FVector(-400.0f,-80.0f,50.0f)));
	PortalesDataBase.Add(FPortalData("Map_Test", FVector(-1200.0f, -80.0f, 50.0f)));
}	
// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
}
void APortalManager::SpawnPortalesCupHead() {
	UWorld* World = GetWorld();

	if (World) {
		for (const FPortalData& data : PortalesDataBase) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// generamos el portal físicamente en el mapa
			FTransform SpawnTransform(FRotator::ZeroRotator, data.SpawnLocation);
			ALevelPortal* SpawnedPortal = World->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), SpawnTransform, SpawnParams);

			// asignamos
			if (SpawnedPortal)
			{
				SpawnedPortal->TargetLevelName = data.LevelName;
			}
		}


	}
}

void APortalManager::VolverCupHead(FVector Location)
{
	if(GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform(FRotator::ZeroRotator, Location + FVector(300.0f, 0.0f, 50.0f));

		ALevelPortal* PortalCup = GetWorld()->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), SpawnTransform, SpawnParams);

		if (PortalCup)
		{
			PortalCup->TargetLevelName = FName(TEXT("Map_CupHeadMap"));
		}
	}
}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

