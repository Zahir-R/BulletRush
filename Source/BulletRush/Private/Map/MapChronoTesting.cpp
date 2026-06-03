// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapChronoTesting.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AMapChronoTesting::AMapChronoTesting()
{
	PrimaryActorTick.bCanEverTick = false;
	MapMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MapMesh"));
	RootComponent = MapMeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MapMesh(TEXT("/Game/free-skybox-space-nebula/source/basic_skybox_3d_flip.basic_skybox_3d_flip"));
	if (MapMesh.Succeeded())
	{
		MapMeshComponent->SetStaticMesh(MapMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MapMaterial(TEXT("/Game/free-skybox-space-nebula/textures/space_nebula_6k_Mat.space_nebula_6k_Mat"));

	if (MapMaterial.Succeeded())
	{
		MapMeshComponent->SetMaterial(0, MapMaterial.Object);
	}

	MapMeshComponent->SetMobility(EComponentMobility::Static);
	MapMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AMapChronoTesting::BeginPlay()
{
	Super::BeginPlay();
	
}


