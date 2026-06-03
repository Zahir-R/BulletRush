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

	void SetShielded(bool bNewShielded);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartAttack() override;
protected:

	UPROPERTY()
	AActor* OwnerBoss;

	UPROPERTY()
	bool bShielded;
};