// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WeaponBaseComponent.h"
#include "ChargedWeaponComponent.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class BULLETRUSH_API UChargedWeaponComponent : public UWeaponBaseComponent
{
	GENERATED_BODY()
	
protected:
	void StartFiring() override;
	void StopFiring() override;

	FTimerHandle FiringTimer;

	float DamageBase = 10.0f;
	float Damage = 10.0f;
	float MaxDamage = 160.0f;

	/*void ExecuteFire(); */
};
