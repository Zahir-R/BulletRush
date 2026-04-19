// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjects/Planet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	/*RangeSphere = CreateDefaultSubobject<USphereComponent>("Range");
	RangeSphere->SetupAttachment(RootComponent);
	RangeSphere->SetSphereRadius(400.f);*/
}

void APlanet::BeginPlay()
{
	Super::BeginPlay();
	/*
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &APlanet::OnEnterRange);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &APlanet::OnExitRange);*/
}

/*void APlanet::OnEnterRange(...)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		9999.f,
		FColor::Green,
		PlanetName.ToString() + "\n" +
		Description.ToString() +
		"\nPress E to Enter"
	);
}
void APlanet::OnExitRange(...)
{
	GEngine->ClearOnScreenDebugMessages();
}

*/