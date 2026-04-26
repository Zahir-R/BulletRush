// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownGameMode.h"
#include "../Public/Player/TopDownPlayer.h"
#include "PortalManager.h"

ATopDownGameMode::ATopDownGameMode()
{
	DefaultPawnClass = ATopDownPlayer::StaticClass();
}
/*
void ATopDownGameMode::BeginPlay()
{
	Super::BeginPlay();
	//Configuramos las reglas de spawn para que se ignore las colisiones
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//spawneamos el portalmanger
	if (GetWorld())
	{
		GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}
}
*/