// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TopDownGameMode.h"
#include "Player/TopDownPlayer.h"
#include "Map/PortalManager.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ATopDownGameMode::ATopDownGameMode()
{
	DefaultPawnClass = ATopDownPlayer::StaticClass();

	static ConstructorHelpers::FObjectFinder<USoundBase> AmbientFinder(TEXT("SoundWave'/Game/Audio/Ambient.Ambient'"));
	if (AmbientFinder.Succeeded()) AmbientSong = AmbientFinder.Object;
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

	if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
		Music->PlaySong(AmbientSong, 0.0f, 5.0f);
}
