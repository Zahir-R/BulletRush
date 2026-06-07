// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BulletRushGameModeBase.generated.h"

class APowerUpManager;
class APortalManager;
/**
 * 
 */


UCLASS(Blueprintable)
class BULLETRUSH_API ABulletRushGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABulletRushGameModeBase();
	virtual void BeginPlay() override;

	APortalManager* APortalManagerRef;
	static APowerUpManager* SpawnPowerUpsForLevel(UWorld* World, FName LevelName);

	/*
	// Esta funci�n permite decidir qu� Pawn usar din�micamente
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	*/
};
