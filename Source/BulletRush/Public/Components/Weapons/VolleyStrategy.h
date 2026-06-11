// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponsInterface.h"
#include "Sound/SoundBase.h"
#include "VolleyStrategy.generated.h"

class USoundBase;
UCLASS()
class BULLETRUSH_API UVolleyStrategy
	: public UObject,
	public IWeaponsInterface
{
	GENERATED_BODY()

public:
	UVolleyStrategy();

	float FireRate = 0.05f;

	int MaxShots = 20;

	float RechargeTime = 1.f;

	int CurrentShots = 0;

	bool bRecharging = false;

	FTimerHandle RechargeTimer;

	virtual void StartFiring(
		UWeaponBaseComponent* Weapon
	) override;

	virtual void StopFiring(
		UWeaponBaseComponent* Weapon
	) override;

	virtual void ExecuteFire(
		UWeaponBaseComponent* Weapon
	) override;

	void FinishRecharge();

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FireSound;
};