// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Level2GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayingPlayer.h"
#include "Engine/World.h"

ALevel2GameMode::ALevel2GameMode()
{
	FacadeClass = AVaultKeeperFacade::StaticClass();
	LevelFacade = nullptr;
	DefaultPawnClass = APlayingPlayer::StaticClass();
}

void ALevel2GameMode::BeginPlay()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AActor* FoundFacade = UGameplayStatics::GetActorOfClass(World, AVaultKeeperFacade::StaticClass());

    if (FoundFacade)
    {
        LevelFacade = Cast<AVaultKeeperFacade>(FoundFacade);
        UE_LOG(LogTemp, Display, TEXT("[Level2GameMode] Se encontró una Fachada existente en el mapa. Usando esa."));
    }
    else
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        LevelFacade = World->SpawnActor<AVaultKeeperFacade>(FacadeClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        UE_LOG(LogTemp, Display, TEXT("[Level2GameMode] Mapa limpio. Fachada instanciada dinámicamente."));
    }

    // Arrancamos el juego de forma segura sin duplicados
    if (LevelFacade)
    {
        LevelFacade->StartLevelProgression();
    }
}