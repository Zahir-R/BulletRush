// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TestFacade.h"
#include "../../Public/Enemies/BossBase.h"
#include "../../Public/Enemies/EnemyBase.h"
#include "../../Public/Player/PlayingPlayer.h"
#include "Engine/World.h"

// Sets default values
ATestFacade::ATestFacade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	World = GetWorld();


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

	World->SpawnActor<ABossBase>(ABossBase::StaticClass(), Location, FRotator::ZeroRotator);
}
void ATestFacade::SummonEnemies(FVector Center, int NoEnemies)
{
	// if (NoEnemies < 5)	return;
	for (int i = 0; i < NoEnemies; i++)
	{
		FVector NewLocation = Center;
		NewLocation.Y += i * 200;
		World->SpawnActor<AEnemyBase>(AEnemyBase::StaticClass(), NewLocation, FRotator::ZeroRotator);
	}

}