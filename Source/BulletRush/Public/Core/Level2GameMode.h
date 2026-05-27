// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VaultKeeper/core/VaultKeeperFacade.h"
#include "Level2GameMode.generated.h"

UCLASS()
class BULLETRUSH_API ALevel2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALevel2GameMode();

protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY()
	AVaultKeeperFacade* LevelFacade;

	UPROPERTY(EditDefaultsOnly, Category = "Level2|Setup")
	TSubclassOf<AVaultKeeperFacade> FacadeClass;
};