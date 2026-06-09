// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "TimerManager.h"

#include "Enemies/Euclidian/Planes.h"
#include "Tesseriel.generated.h"

class UTesserielBuilder;
class UTesserielStrategy;
class UTesserielImmuneStrategy;
class UTesserielVulnerableStrategy;
class UTesserielDeadStrategy;
class APlanes;

UCLASS()
class BULLETRUSH_API ATesseriel : public ABossBase
{
	GENERATED_BODY()

public:

	ATesseriel();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void Attack() override;

	virtual void Die() override;

	UPROPERTY()
	UTesserielStrategy* CurrentStrategy;

	UPROPERTY()
	UTesserielImmuneStrategy* ImmuneStrategy;

	UPROPERTY()
	UTesserielVulnerableStrategy* VulnerableStrategy;

	UPROPERTY()
	UTesserielDeadStrategy* DeadStrategy;

	UPROPERTY()
	int32 RemainingVulnerablePlanes;

	UPROPERTY()
	UTesserielBuilder* Builder;

	UPROPERTY()
	TArray<APlanes*> ShieldedPlanes;

	UPROPERTY()
	TArray<APlanes*> VulnerablePlanes;

	void OnPlaneDestroyed(APlanes* Plane);

	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;
};