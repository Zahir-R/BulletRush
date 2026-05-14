// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TestGameModeBase.h"
#include "../../Public/Core/TestFacade.h"
#include "../../Public/Player/PlayingPlayer.h"

ATestGameModeBase::ATestGameModeBase()
{
	Arquitect = CreateDefaultSubobject<ATestFacade>(TEXT("Arquitect"));
	DefaultPawnClass = APlayingPlayer::StaticClass();
}

void ATestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Arquitect->SummonBoss(FVector(300.0f, 0.0f, 25.0f));

	Arquitect->SummonEnemies(FVector(800.0f, -400.0f, 25.0f), 2);

}