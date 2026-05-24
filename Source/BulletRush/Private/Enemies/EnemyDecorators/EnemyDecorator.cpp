// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyDecorators/EnemyDecorator.h"
#include "Enemies/EnemyBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
AEnemyDecorator::AEnemyDecorator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CustomMesh = nullptr;
}

void AEnemyDecorator::Decorate(AEnemyBase* NewEnemy)
{
	Enemy = NewEnemy;

	ApplyDecoration();
}
void AEnemyDecorator::ApplyDecoration() {
	if (Enemy && Enemy->MeshEnemy && CustomMesh)
	{
		Enemy->MeshEnemy->SetStaticMesh(CustomMesh);
	}
}

