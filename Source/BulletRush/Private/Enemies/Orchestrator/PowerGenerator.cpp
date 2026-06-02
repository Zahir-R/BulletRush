// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/PowerGenerator.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Core/Orchestrator/OrchestratorFacade.h"
#include "Kismet/GameplayStatics.h"

APowerGenerator::APowerGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->MaxHealth = 500.0f;

	Tags.Add(FName("Generador"));
	Tags.Add("Enemigo");
}

void APowerGenerator::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->CurrentHealth = HealthComp->MaxHealth;
	HealthComp->OnDeath.AddDynamic(this, &APowerGenerator::OnGeneratorDestroyed);
}

float APowerGenerator::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HealthComp)
	{
		return HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return 0.0f;
}

void APowerGenerator::OnGeneratorDestroyed()
{
	// Reportamos la destrucción a la Fachada
	if (AOrchestratorGameMode* GM = Cast<AOrchestratorGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (GM->LevelFacade)
		{
			GM->LevelFacade->ReportGeneratorDestroyed();
		}
	}
	Destroy();
}

