// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhaseObjectiveInterface.h"
#include "BossObjective.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UBossObjective
    : public UObject,
    public IPhaseObjectiveInterface
{
    GENERATED_BODY()

private:

    bool bBossKilled = false;

public:

    virtual void Initialize(
        AEuclidianGameMode* GameMode) override;

    virtual void OnEnemyKilled(
        AEnemyBase* Enemy) override;

    virtual bool IsCompleted() const override;
};