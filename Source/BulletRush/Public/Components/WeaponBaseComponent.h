// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weaponsinterface.h"
#include "Components/SceneComponent.h"
#include "WeaponBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UWeaponBaseComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY()
	TScriptInterface<IWeaponsInterface> FireStrategy;

	virtual void StartFiring();
	virtual void StopFiring();

	float FireRate = .1f;

	FTimerHandle FiringTimer;

	float BaseDamage = 20.0f;

	float BaseProjectileSpeed = 7500.f;
	
	void SetFireStrategy(
		TScriptInterface<IWeaponsInterface> NewStrategy
	);
};
