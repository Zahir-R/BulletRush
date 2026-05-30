// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TestFacade.h"
#include "../../Public/Enemies/BossBase.h"
#include "../../Public/Enemies/EnemyBase.h"
#include "../../Public/Player/PlayingPlayer.h"
#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Engine/World.h"

// Sets default values
ATestFacade::ATestFacade()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestFacade::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATestFacade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestFacade::SummonBoss(FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return;
	World->SpawnActor<ABossBase>(ABossBase::StaticClass(), Location, FRotator::ZeroRotator);
}
void ATestFacade::SummonEnemies(FVector Center, int NoEnemies)
{
	UWorld* World = GetWorld();
	if (!World) return;
	for (int i = 0; i < NoEnemies; i++)
	{
		FVector NewLocation = Center;
		NewLocation.Y += i * 200;
		World->SpawnActor<AEnemyBase>(AEnemyBase::StaticClass(), NewLocation, FRotator::ZeroRotator);
	}

}

void ATestFacade::SummonLinker(FVector Location)
{
	UWorld* World = GetWorld();
	if (!World) return;
	AChronostasisLinker* Linker = World->SpawnActor<AChronostasisLinker>(AChronostasisLinker::StaticClass(), Location, FRotator::ZeroRotator);
	if (Linker) UE_LOG(LogTemp, Warning, TEXT("Linker spawneado en %s"), *Location.ToString());
}