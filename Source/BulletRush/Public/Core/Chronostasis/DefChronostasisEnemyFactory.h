// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Chronostasis/ChronostasisFactoryEnemy.h"
#include "DefChronostasisEnemyFactory.generated.h"

class AEnemyBase;

UCLASS(Blueprintable)
class BULLETRUSH_API UDefChronostasisEnemyFactory : public UChronostasisFactoryEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory | Classes")
	TSubclassOf<AEnemyBase> DroneClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory | Classes")
	TSubclassOf<AEnemyBase> MassClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory | Classes")
	TSubclassOf<AEnemyBase> ExpansiveClass;

public:
	virtual AEnemyBase* CreateDrone(UWorld* World, const FVector& Location) override;
	virtual AEnemyBase* CreateMass(UWorld* World, const FVector& Location) override;
	virtual AEnemyBase* CreateExpansive(UWorld* World, const FVector& Location) override;
};