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
    UE_LOG(LogTemp, Warning, TEXT("APowerUpManager::BeginPlay: Actor %s in level %s. PowerUpClasses=%d"), *GetName(), *GetWorld()->GetMapName(), PowerUpClasses.Num());
	float FirstDelay = FMath::RandRange(MinSpawnTime, MaxSpawnTime);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &APowerUpManager::SpawnRandomPowerUp, FirstDelay, false);
}

void APowerUpManager::SpawnRandomPowerUp()
{
    // Only verify we have a world. Previously this also checked IsGameWorld which
	// caused powerups to only spawn on a specific test map in this project setup.
	// Remove that restriction so powerups can spawn on any level played in-game.
	if (!GetWorld()) {
		ScheduleNextSpawn();
		return;
	}
    UE_LOG(LogTemp, Verbose, TEXT("APowerUpManager::SpawnRandomPowerUp called on %s"), *GetName());
	SpawnedPowerUps.RemoveAll([this](APowerUpBase* P)
		{
			if (!IsValid(P))
			{
				for (auto It = LifecycleTimers.CreateIterator(); It; ++It)
				{
					if (It.Key() == P)
					{
						GetWorldTimerManager().ClearTimer(It.Value());
						It.RemoveCurrent();
						break;
					}
				}
				return true;
			}
			return false;
		});

	if (!SpawnArea)
	{
		UE_LOG(LogTemp, Warning, TEXT("APowerUpManager: No SpawnArea configured on %s"), *GetName());
		ScheduleNextSpawn();
		return;
	}

	if (PowerUpClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("APowerUpManager: No PowerUpClasses configured on %s"), *GetName());
		ScheduleNextSpawn();
		return;
	}

	if (SpawnedPowerUps.Num() >= MaxPowerUps)
	{
        UE_LOG(LogTemp, Verbose, TEXT("APowerUpManager: MaxPowerUps reached on %s"), *GetName());
		ScheduleNextSpawn();
		return;
	}

	int32 Index = FMath::RandRange(0, PowerUpClasses.Num() - 1);
	TSubclassOf<APowerUpBase> ChosenClass = PowerUpClasses[Index];
	if (!ChosenClass) return;

	FBoxSphereBounds Bounds = SpawnArea->Bounds;
	FVector SpawnLoc = Bounds.Origin + FMath::RandPointInBox(FBox(-Bounds.BoxExtent, Bounds.BoxExtent));

	// TODO while (estaOtroObjeto)
		// FVector SpawnLoc = Bounds.Origin + FMath::RandPointInBox(FBox(-Bounds.BoxExtent, Bounds.BoxExtent));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APowerUpBase* NewPowerUp = GetWorld()->SpawnActor<APowerUpBase>(ChosenClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	if (NewPowerUp)
	{
		NewPowerUp->SetManager(this);
		SpawnedPowerUps.Add(NewPowerUp);
		if (PowerUpLifetime > 0.0f)
		{
			FTimerHandle LifeTimer;
			TWeakObjectPtr<APowerUpBase> WeakPowerUp = NewPowerUp;
			GetWorldTimerManager().SetTimer(LifeTimer, FTimerDelegate::CreateLambda([this, WeakPowerUp, LifeTimer]()
				{
					if (WeakPowerUp.IsValid() && !WeakPowerUp->IsActorBeingDestroyed())
					{
						OnPowerUpCollected(WeakPowerUp.Get());
						WeakPowerUp->Destroy();
					}
					else
					{
						for (auto It = LifecycleTimers.CreateIterator(); It; ++It)
						{
							if (It.Value() == LifeTimer)
							{
								It.RemoveCurrent();
								break;
							}
						}
					}
				}), PowerUpLifetime, false);
			LifecycleTimers.Add(NewPowerUp, LifeTimer);
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
	if (FTimerHandle* Handle = LifecycleTimers.Find(PowerUp))
	{
		GetWorldTimerManager().ClearTimer(*Handle);
		LifecycleTimers.Remove(PowerUp);
	}
	SpawnedPowerUps.Remove(PowerUp);
}

void APowerUpManager::Initialize(const TArray<TSubclassOf<APowerUpBase>>& Classes, const FVector& SpawnAreaExtent)
{
	PowerUpClasses = Classes;
	if (SpawnArea)
	{
		SpawnArea->SetBoxExtent(SpawnAreaExtent);
	}
}

void APowerUpManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& Pair : LifecycleTimers)
	{
		GetWorldTimerManager().ClearTimer(Pair.Value);
	}
	LifecycleTimers.Empty();
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	Super::EndPlay(EndPlayReason);
}