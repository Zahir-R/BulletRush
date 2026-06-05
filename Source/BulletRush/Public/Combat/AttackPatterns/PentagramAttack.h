// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "PentagramAttack.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UPentagramAttack : public UAttackStrategy
{
	GENERATED_BODY()
public:
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};
