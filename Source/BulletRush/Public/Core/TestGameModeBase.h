// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestGameModeBase.generated.h"

class ATestFacade;

/**
 * 
 */
UCLASS()
class BULLETRUSH_API ATestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

	ATestFacade* Arquitect;
};
