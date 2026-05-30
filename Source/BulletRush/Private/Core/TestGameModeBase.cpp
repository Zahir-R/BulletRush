// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/TestGameModeBase.h"
#include "Core/TestFacade.h"
#include "Core/Requirements/RequirementManager.h"
#include "Core/Requirements/CollectibleRequirement.h"
#include "Test/Collectible.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"

ATestGameModeBase::ATestGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
}

void ATestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	RequirementManager = NewObject<URequirementManager>(this);
	RequirementManager->RegisterComponent();

	UCollectibleRequirement* Collect = NewObject<UCollectibleRequirement>(RequirementManager);
	Collect->RequiredCount = 3;
	RequirementManager->SecretRequirements.Add(Collect);

	FTimerHandle InitTimer;
	GetWorldTimerManager().SetTimer(InitTimer, this, &ATestGameModeBase::InitializeRequirements, 0.1f, false);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Arquitect = GetWorld()->SpawnActor<ATestFacade>(ATestFacade::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Arquitect)
	{
		Arquitect->SummonBoss(FVector(300.0f, 0.0f, 25.0f));
		Arquitect->SummonEnemies(FVector(800.0f, -400.0f, 25.0f), 2);
		Arquitect->SummonLinker(FVector(300, 100, 25));
	}

	TArray<FVector> CollectibleLocs;
	CollectibleLocs.Add(FVector(500.0f, 0.0f, 50.0f));
	CollectibleLocs.Add(FVector(500.0f, 100.0f, 50.0f));
	CollectibleLocs.Add(FVector(500.0f, 200.0f, 50.0f));
	SpawnCollectibles(CollectibleLocs);
}

void ATestGameModeBase::InitializeRequirements()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && RequirementManager)
	{
		RequirementManager->InitializeRequirements(PC);
		UE_LOG(LogTemp, Warning, TEXT("Reqs inicializados"));
	}
	else
	{
		FTimerHandle RetryTimer;
		GetWorldTimerManager().SetTimer(RetryTimer, this, &ATestGameModeBase::InitializeRequirements, 0.1f, false);
	}
}

void ATestGameModeBase::SpawnCollectibles(const TArray<FVector>& Locations)
{
	UWorld* World = GetWorld();
	if (!World) return;
	for (const FVector& Loc : Locations)
	{
		ACollectible* Collectible = World->SpawnActor<ACollectible>(ACollectible::StaticClass(), Loc, FRotator::ZeroRotator);
		if (Collectible)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned collectible at %s"), *Loc.ToString());
		}
	}
}