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
	
	ABulletRushGameModeBase();
public:
	virtual void BeginPlay() override;
	// Esta función permite decidir qué Pawn usar dinámicamente
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UFUNCTION(Exec)
	void DealDamageToTarget(float Damage);
};
