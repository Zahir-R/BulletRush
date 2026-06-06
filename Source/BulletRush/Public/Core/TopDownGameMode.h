// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TopDownGameMode.generated.h"

class APortalManager;
/**
 * 
 */
UCLASS(Blueprintable)
class BULLETRUSH_API ATopDownGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATopDownGameMode();
	
	UPROPERTY()
	APortalManager* PortalManager;

public:
	virtual void BeginPlay() override;
	
	
};
