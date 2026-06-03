// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhaseObjectiveInterface.h"
#include "RedTurretObjective.generated.h"

UCLASS()
class BULLETRUSH_API URedTurretObjective
	: public UObject
	, public IPhaseObjectiveInterface
{
	GENERATED_BODY()

public:

	int32 DeadTurrets = 0;

	virtual void Initialize(AEuclidianGameMode* GameMode) override;

	virtual void OnEnemyKilled(AEnemyBase* Enemy) override;

	virtual bool IsCompleted() const override;
};
