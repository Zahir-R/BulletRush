	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/Common/Drone.h"
#include "RedDrone.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API ARedDrone : public ADrone
{
	GENERATED_BODY()

public:
	ARedDrone();

	virtual void Die() override;
};
