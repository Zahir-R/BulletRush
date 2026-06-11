// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/WeaponBaseComponent.h"
#include "Player/PlayingPlayer.h"
#include "../../Public/Subsystems/ProjectilesSubsystem.h"

// Sets default values for this component's properties
UWeaponBaseComponent::UWeaponBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponBaseComponent::StartFiring()
{
	if (FireStrategy)
	{
		FireStrategy->StartFiring(this);
	}
}

void UWeaponBaseComponent::StopFiring()
{
	if (FireStrategy)
	{
		FireStrategy->StopFiring(this);
	}
}

void UWeaponBaseComponent::SetFireStrategy(
	TScriptInterface<IWeaponsInterface> NewStrategy)
{
	FireStrategy = NewStrategy;
}

