// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Planes.generated.h"

class ATesseriel;

UCLASS()
class BULLETRUSH_API APlanes : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	APlanes();

	void SetBoss(AActor* NewBoss);

	virtual void Die() override;

	void SetShielded(bool bNewShielded);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartAttack() override;
	void InitializeOrbit(
		float StartAngle,
		float Radius
	);
	bool bOrbitX = false;
	bool bOrbitY = false;

	UPROPERTY()
	FRotator FaceRotation;
protected:

	UPROPERTY()
	AActor* OwnerBoss;

	UPROPERTY()
	bool bShielded;

	float OrbitAngle;

	float OrbitRadius;

	float OrbitSpeed;
};