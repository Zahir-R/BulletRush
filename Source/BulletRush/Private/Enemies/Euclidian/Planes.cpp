// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/Euclidian/Planes.h"
#include "Enemies/Euclidian/Tesseriel.h"
//#include "Enemies/Euclidian/Tesseriel.h"

APlanes::APlanes()
{
	bShielded = false;
	OwnerBoss = nullptr;
	AttackInterval = 1.0f;
	bAutoStartAttack = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshAsset.Succeeded())
	{
		MeshEnemy->SetStaticMesh(MeshAsset.Object);

		MeshEnemy->SetRelativeScale3D(
			FVector(
				1.9f,  // Length
				1.9f,  // Width
				0.05f   // Thickness
			)
		);
	}

	OrbitAngle = 0.f;
	OrbitRadius = 500.f;
	OrbitSpeed = 30.f;
}
void APlanes::BeginPlay()
{
	Super::BeginPlay();

	BeginAttackLoop();
}
void APlanes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((bOrbitX && OwnerBoss)|| (bOrbitY && OwnerBoss))
	{
		OrbitAngle += OrbitSpeed * DeltaTime;

		FVector BossLocation =
			OwnerBoss->GetActorLocation();

		FVector Offset(
			FMath::Cos(
				FMath::DegreesToRadians(OrbitAngle)
			) * OrbitRadius,

			FMath::Sin(
				FMath::DegreesToRadians(OrbitAngle)
			) * OrbitRadius,

			50.f
		);

		SetActorLocation(
			BossLocation + Offset
		);
	}
	if (bOrbitX)
	{
		SetActorRotation(
			FRotator(
				90.f,
				OwnerBoss->GetActorRotation().Yaw,
				0.f
			)
		);
	}
	else if (bOrbitY)
	{
		SetActorRotation(
			FRotator(
				0.f,
				OwnerBoss->GetActorRotation().Yaw,
				90.f
			)
		);
	}
	else
	{
		SetActorRotation(
			OwnerBoss->GetActorRotation()
		);
	}
}
void APlanes::SetBoss(
	AActor* NewBoss)
{
	OwnerBoss = NewBoss;
}
void APlanes::SetShielded(
	bool bNewShielded)
{
	bShielded = bNewShielded;

	SetInvulnerability(
		bShielded
	);
}
void APlanes::StartAttack()
{
	if (!BulletSpawner)
	{
		return;
	}

	if (!OwnerBoss)
	{
		return;
	}

	FVector Dir =
		(GetActorLocation() -
			OwnerBoss->GetActorLocation())
		.GetSafeNormal();

	BulletSpawner->InternalSpawn(
		GetActorLocation(),
		Dir,
		1000.f,
		20.f
	);
}
void APlanes::InitializeOrbit(
	float StartAngle,
	float Radius)
{
	OrbitAngle = StartAngle;
	OrbitRadius = Radius;
}
void APlanes::Die()
{
	if (OwnerBoss)
	{
		if (ATesseriel* Boss =
			Cast<ATesseriel>(OwnerBoss))
		{
			Boss->OnPlaneDestroyed(this);
		}
	}

	Super::Die();
}