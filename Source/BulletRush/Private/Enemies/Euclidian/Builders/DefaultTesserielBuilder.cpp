// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Euclidian/Builders/DefaultTesserielBuilder.h"
#include "Enemies/Euclidian/Tesseriel.h"
#include "Enemies/Euclidian/Planes.h"
#include "Engine/World.h"

void UDefaultTesserielBuilder::BuildBoss(
	ATesseriel* Boss)
{
	if (!Boss)
	{
		return;
	}

	UWorld* World = Boss->GetWorld();

	if (!World)
	{
		return;
	}
	
	TArray<FVector> SpawnOffset =
	{
		FVector(191.f,  0.f, 0.f),
		FVector(0.f, -191.f, 0.f),
		FVector(-191.f,  0.f, 0.f),
		FVector(0.f, 191.f, 0.f),
		FVector(0.f, 0.f, 191.f),
		FVector(0.f, 0.f, -116.f)
	};
    for (int32 i = 0; i < SpawnOffset.Num(); i++)
    {
        APlanes* Plane =
            World->SpawnActor<APlanes>(
                APlanes::StaticClass(),
                Boss->GetActorLocation() + SpawnOffset[i],
                FRotator::ZeroRotator
            );

        if (!Plane)
        {
            continue;
        }

        Plane->SetBoss(Boss);

        if (i < 4)
        {
            if (i % 2 == 0) {
                Plane->bOrbitX = true;
            }
            else {
                Plane->bOrbitY = true;
            }
            Plane->InitializeOrbit(
                i * 90.f,
                191.f
            );
        }

        if (i < 3)
        {
            Plane->SetShielded(true);
            Boss->ShieldedPlanes.Add(Plane);
        }
        else
        {
            Plane->SetShielded(false);
            Boss->VulnerablePlanes.Add(Plane);
        }
    }
}