// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "LevelPortal.h"
#include "Engine/World.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalesDataBase.Add(FPortalData("Level1", FVector(0, 0, 0)));
	PortalesDataBase.Add(FPortalData("level12", FVector(0,0,0)));
	PortalesDataBase.Add(FPortalData("level12", FVector(0,0,0)));
	PortalesDataBase.Add(FPortalData("level12", FVector(0,0,0)));
	PortalesDataBase.Add(FPortalData("level12", FVector(0,0,0)));


}	
// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();

	if (World) {
		for (const FPortalData& data : PortalesDataBase) {
		
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// generamos el portal físicamente en el mapa
			ALevelPortal* SpawnedPortal = World->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), data.SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			// asignamos
			if (SpawnedPortal)
			{
				SpawnedPortal->TargetLevelName = data.LevelName;
			}
		}
	
	
	}


}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

