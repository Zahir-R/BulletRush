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
	if (!GetWorld() || !GetWorld()->IsGameWorld()) {
		ScheduleNextSpawn();
		return;
	}
	SpawnedPowerUps.RemoveAll([](APowerUpBase* P)
		{
			return !IsValid(P);
		});

	if (!SpawnArea || PowerUpClasses.Num() == 0 || SpawnedPowerUps.Num() >= MaxPowerUps)
	{
		ScheduleNextSpawn();
		return;
	}

	int32 Index = FMath::RandRange(0, PowerUpClasses.Num() - 1);
	TSubclassOf<APowerUpBase> ChosenClass = PowerUpClasses[Index];
	if (!ChosenClass) return;

	FBoxSphereBounds Bounds = SpawnArea->Bounds;
	FVector SpawnLoc = Bounds.Origin + FMath::RandPointInBox(FBox(-Bounds.BoxExtent, Bounds.BoxExtent));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APowerUpBase* NewPowerUp = GetWorld()->SpawnActor<APowerUpBase>(ChosenClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	if (NewPowerUp)
	{
		SpawnedPowerUps.Add(NewPowerUp);
		if (PowerUpLifetime > 0.0f)
		{
			FTimerHandle LifeTimer;
			GetWorldTimerManager().SetTimer(LifeTimer, [this, NewPowerUp]()
				{
					if (IsValid(NewPowerUp))
					{
						OnPowerUpCollected(NewPowerUp);
						NewPowerUp->Destroy();
					}
				}, PowerUpLifetime, false);
		}
	}

	ScheduleNextSpawn();
}

void APowerUpManager::ScheduleNextSpawn()
{
	float NextDelay = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnRandomPowerUp, NextDelay, false);
}

void APowerUpManager::OnPowerUpCollected(APowerUpBase* PowerUp)
{
	if (!PowerUp) return;
	SpawnedPowerUps.Remove(PowerUp);
}

void APowerUpManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	Super::EndPlay(EndPlayReason);
}