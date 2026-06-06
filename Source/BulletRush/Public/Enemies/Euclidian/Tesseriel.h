// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"

#include "Enemies/Euclidian/Planes.h"
#include "Tesseriel.generated.h"

class UTesserielBuilder;
class UTesserielStrategy;
class UTesserielImmuneStrategy;
class UTesserielVulnerableStrategy;
class UTesserielDeadStrategy;
class APlanes;

UCLASS()
class BULLETRUSH_API ATesseriel : public ABossBase
{
	GENERATED_BODY()

public:

	ATesseriel();

	virtual void BeginPlay() override;

	UPROPERTY()
	UTesserielStrategy* CurrentStrategy;

	UPROPERTY()
	UTesserielImmuneStrategy* ImmuneStrategy;

	UPROPERTY()
	UTesserielVulnerableStrategy* VulnerableStrategy;

	UPROPERTY()
	UTesserielDeadStrategy* DeadStrategy;

	UPROPERTY()
	UTesserielBuilder* Builder;

	UPROPERTY()
	TArray<APlanes*> ShieldedPlanes;

	UPROPERTY()
	TArray<APlanes*> VulnerablePlanes;
};