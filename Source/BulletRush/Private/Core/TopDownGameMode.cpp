// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TopDownGameMode.h"
#include "Player/TopDownPlayer.h"
#include "Map/PortalManager.h"

ATopDownGameMode::ATopDownGameMode()
{
	DefaultPawnClass = ATopDownPlayer::StaticClass();
}

void ATopDownGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld())
	{
	//Configuramos las reglas de spawn para que se ignore las colisiones
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		PortalManager = GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
	if (PortalManager) {
		PortalManager->SpawnPortalesCupHead();
	}
}
