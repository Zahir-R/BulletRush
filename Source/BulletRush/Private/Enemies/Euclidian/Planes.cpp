// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/Euclidian/Planes.h"
//#include "Enemies/Euclidian/Tesseriel.h"

APlanes::APlanes()
{
	bShielded = false;
	OwnerBoss = nullptr;
	AttackInterval = 1.0f;
	bAutoStartAttack = false;
}
void APlanes::BeginPlay()
{
	Super::BeginPlay();

	BeginAttackLoop();
}
void APlanes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OwnerBoss)
	{
		return;
	}

	FVector Direction =
		GetActorLocation() -
		OwnerBoss->GetActorLocation();

	SetActorRotation(
		FRotator(
			0.f,
			Direction.Rotation().Yaw,
			0.f
		)
	);
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