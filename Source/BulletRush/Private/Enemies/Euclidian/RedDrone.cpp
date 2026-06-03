// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Euclidian/RedDrone.h"
#include "Enemies/Euclidian/RedTurretBuffPickup.h"

ARedDrone::ARedDrone()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		MaterialAsset(
			TEXT("Material'/Game/StarterContent/Materials/M_Wood_Walnut.M_Wood_Walnut'")
		);

	if (MaterialAsset.Succeeded() && MeshEnemy)
	{
		MeshEnemy->SetMaterial(
			0,
			MaterialAsset.Object
		);
	}
}
void ARedDrone::Die()
{
	GetWorld()->SpawnActor<ARedTurretBuffPickup>(
		ARedTurretBuffPickup::StaticClass(),
		GetActorLocation(),
		FRotator::ZeroRotator
	);

	Super::Die();
}