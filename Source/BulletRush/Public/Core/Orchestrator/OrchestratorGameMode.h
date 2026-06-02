// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/BulletRushGameModeBase.h"
#include "OrchestratorGameMode.generated.h"

class AOrchestratorFacade;
/**
 * 
 */
UCLASS()
class BULLETRUSH_API AOrchestratorGameMode : public ABulletRushGameModeBase
{
	GENERATED_BODY()
	
public:
	AOrchestratorGameMode();

	virtual void BeginPlay() override;

	// Referencia pública a la Fachada para que los actores puedan comunicarse
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Progression")
	AOrchestratorFacade* LevelFacade;
};
