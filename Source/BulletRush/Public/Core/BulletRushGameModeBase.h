// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BulletRushGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API ABulletRushGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
<<<<<<< HEAD:Source/BulletRush/Public/Core/BulletRushGameModeBase.h
	
	ABulletRushGameModeBase();
public:
	// Esta función permite decidir qué Pawn usar dinámicamente
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
=======
public:
>>>>>>> bullet_and_pool:Source/BulletRush/BulletRushGameModeBase.h
};
