// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDecorator.generated.h"

class AEnemyBase;
UCLASS()
class BULLETRUSH_API AEnemyDecorator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDecorator();

	UPROPERTY()
	AEnemyBase* Enemy;

	virtual void ApplyDecoration();

	void Decorate(AEnemyBase* NewEnemy);
};