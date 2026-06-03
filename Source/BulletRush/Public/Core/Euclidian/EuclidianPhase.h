// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EuclidianPhase.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UEuclidianPhase : public UObject
{	
	GENERATED_BODY()

public:
	virtual void EnterPhase(class AEuclidianGameMode* GameMode);
	virtual void ExitPhase(class AEuclidianGameMode* GameMode);
};