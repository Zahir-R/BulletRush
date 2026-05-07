// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Core/TopDownGameMode.h"
#include "../Public/Player/TopDownPlayer.h"
#include "../../Public/Map/PortalManager.h"

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
		GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
}
