// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/BossArenaTrigger.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABossArenaTrigger::ABossArenaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Creamos la raíz vacía y la asignamos
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// 2. Creamos el TriggerBox y lo adjuntamos a la raíz
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->InitBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	BossSpawnOffset = FVector(8200.0f, -2250.0f, 1000.0f);
}

void ABossArenaTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossArenaTrigger::OnOverlapBegin);
}

void ABossArenaTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Solo reacciona si el jugador atraviesa la puerta
	if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
	{
		PrepareArena();
	}
}

void ABossArenaTrigger::PrepareArena()
{
	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade)
		{
			// Calculamos donde spawnearemos al jefe
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(BossSpawnOffset);
			SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());

			GM->LevelFacade->PrepareBossArena(SpawnTransform);
			// Nos destruimos para que no se ejecute dos veces
			Destroy();
		}
	}
}