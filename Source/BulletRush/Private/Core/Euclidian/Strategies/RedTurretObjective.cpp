// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Euclidian/Strategies/RedTurretObjective.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Enemies/Euclidian/RedTurret.h"

void URedTurretObjective::Initialize(AEuclidianGameMode* GameMode)
{
	TArray<AActor*> Turrets;

	UGameplayStatics::GetAllActorsOfClass(
		GameMode->GetWorld(),
		ARedTurret::StaticClass(),
		Turrets
	);

	for (AActor* Actor : Turrets)
	{
		ARedTurret* Turret =
			Cast<ARedTurret>(Actor);

		if (Turret)
		{
			Turret->OnEnemyDeath.AddDynamic(
				GameMode,
				&AEuclidianGameMode::OnObservedEnemyDeath
			);
		}
	}
}
void URedTurretObjective::OnEnemyKilled(AEnemyBase* Enemy)
{
	if (Cast<ARedTurret>(Enemy))
	{
		DeadTurrets++;
	}
}

bool URedTurretObjective::IsCompleted() const
{
	return DeadTurrets >= 3;
}