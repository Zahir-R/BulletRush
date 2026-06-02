// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Euclidian/RedTurret.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ARedTurret::ARedTurret()
{
	// Double damage
	Damage = 40.f;
	AttackInterval = 0.25f;
}
void ARedTurret::BeginPlay()	
{
	Super::BeginPlay();
}