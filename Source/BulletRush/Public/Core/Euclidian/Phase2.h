// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Euclidian/EuclidianPhase.h"
#include "Phase2.generated.h"


class AEuclidianGameMode;

UCLASS()
class BULLETRUSH_API UPhase2 : public UEuclidianPhase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATesseriel> BossClass;

	virtual void EnterPhase(AEuclidianGameMode* GameMode) override;
};