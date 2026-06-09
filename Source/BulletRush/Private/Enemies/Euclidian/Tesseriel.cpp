// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/Euclidian/Tesseriel.h"
#include "Enemies/Euclidian/Builders/DefaultTesserielBuilder.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielImmuneStrategy.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielVulnerableStrategy.h"
#include "Enemies/Euclidian/TesserielComponents/TesserielDeadStrategy.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Euclidian/EuclidianGameMode.h"
#include "Components/BulletSpawnerComponent.h"

ATesseriel::ATesseriel()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshAsset.Succeeded())
	{
		MeshEnemy->SetStaticMesh(MeshAsset.Object);

	}
}
void ATesseriel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(
		FRotator(
			0.f,
			30.f * DeltaTime, // degrees per second
			0.f
		)
	);
}
void ATesseriel::BeginPlay()
{
	Super::BeginPlay();

	ImmuneStrategy =
		NewObject<UTesserielImmuneStrategy>(this);

	VulnerableStrategy =
		NewObject<UTesserielVulnerableStrategy>(this);

	DeadStrategy =
		NewObject<UTesserielDeadStrategy>(this);

	CurrentStrategy = ImmuneStrategy;

	Builder =
		NewObject<UDefaultTesserielBuilder>(this);

	Builder->BuildBoss(this);

	RemainingVulnerablePlanes = 3;

	SetInvulnerable(true);
	CurrentStrategy = ImmuneStrategy;
}
void ATesseriel::Attack()
{
	if (!BulletSpawner)
	{
		return;
	}

	APawn* Player =
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!Player)
	{
		return;
	}

	FVector Forward =
		(Player->GetActorLocation() -
			GetActorLocation()).GetSafeNormal();

	const float Angles[7] =
	{
		-30.f,
		-20.f,
		-10.f,
		0.f,
		10.f,
		20.f,
		30.f
	};

	for (float Angle : Angles)
	{
		FVector Dir =
			Forward.RotateAngleAxis(
				Angle,
				FVector::UpVector
			);

		BulletSpawner->InternalSpawn(
			GetActorLocation(),
			Dir,
			1000.f,
			20.f
		);
	}
}
void ATesseriel::OnPlaneDestroyed(
	APlanes* Plane)
{
	if (!Plane)
	{
		return;
	}

	if (VulnerablePlanes.Contains(Plane))
	{
		RemainingVulnerablePlanes--;

		if (RemainingVulnerablePlanes <= 0)
		{
			SetInvulnerable(false);

			CurrentStrategy =
				VulnerableStrategy;
		}
	}
}
float ATesseriel::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	if (!CurrentStrategy)
	{
		return 0.f;
	}

	if (!CurrentStrategy->CanTakeDamage())
	{
		return 0.f;
	}

	return Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser
	);
}
void ATesseriel::Die()
{
	if (AEuclidianGameMode* GM =
		Cast<AEuclidianGameMode>(
			GetWorld()->GetAuthGameMode()))
	{
		GM->OnObservedEnemyDeath(this);
	}

	Super::Die();
}