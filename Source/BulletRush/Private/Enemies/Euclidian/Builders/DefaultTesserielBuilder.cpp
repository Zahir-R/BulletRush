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

	for (int32 i = 0; i < 6; i++)
	{
		float Angle = i * 60.f;

		FVector Offset(
			FMath::Cos(
				FMath::DegreesToRadians(Angle)
			) * 600.f,

			FMath::Sin(
				FMath::DegreesToRadians(Angle)
			) * 600.f,

			0.f
		);

		APlanes* Plane =
			World->SpawnActor<APlanes>(
				APlanes::StaticClass(),
				Boss->GetActorLocation() + Offset,
				FRotator::ZeroRotator
			);

		if (!Plane)
		{
			continue;
		}

		Plane->SetBoss(Boss);

		if (i < 3)
		{
			Plane->SetShielded(true);

			Boss->ShieldedPlanes.Add(
				Plane
			);
		}
		else
		{
			Plane->SetShielded(false);

			Boss->VulnerablePlanes.Add(
				Plane
			);
		}
	}
}