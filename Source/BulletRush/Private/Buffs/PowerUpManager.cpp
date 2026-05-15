#include "Buffs/PowerUpManager.h"
#include "Components/BoxComponent.h"
#include "Buffs/PowerUpBase.h"
#include "Engine/World.h"
#include "Math/RandomStream.h"

APowerUpManager::APowerUpManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;

	SpawnArea->SetBoxExtent(FVector(1000.0f, 1000.0f, 400.0f));
}

void APowerUpManager::BeginPlay()
{
	Super::BeginPlay();
	float FirstDelay = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnRandomPowerUp, FirstDelay, false);
}

void APowerUpManager::SpawnRandomPowerUp()
{
	if (!SpawnArea || PowerUpClasses.Num() == 0)
	{
		float Next = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnRandomPowerUp, Next, false);
		return;
	}

	int32 Index = FMath::RandRange(0, PowerUpClasses.Num() - 1);
	TSubclassOf<APowerUpBase> ChosenClass = PowerUpClasses[Index];
	if (!ChosenClass) return;

	FBoxSphereBounds Bounds = SpawnArea->Bounds;
	FVector SpawnLoc = Bounds.Origin + FMath::RandPointInBox(FBox(-Bounds.BoxExtent, Bounds.BoxExtent));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<APowerUpBase>(ChosenClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	float NextDelay = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnRandomPowerUp, NextDelay, false);

}