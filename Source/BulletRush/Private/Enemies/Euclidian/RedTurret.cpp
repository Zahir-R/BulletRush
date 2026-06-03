// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Euclidian/RedTurret.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Core/Euclidian/EuclidianGameMode.h"

ARedTurret::ARedTurret()
{
	// Double damage
	Damage = 40.f;
	AttackInterval = 0.25f;
}
void ARedTurret::BeginPlay()	
{
	Super::BeginPlay();
}
float ARedTurret::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	AEuclidianGameMode* GM =
		Cast<AEuclidianGameMode>(
			GetWorld()->GetAuthGameMode()
		);

	if (!GM)
	{
		return 0.f;
	}

	if (!GM->bRedTurretsVulnerable)
	{
		return 0.f;
	}

	return Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);
}