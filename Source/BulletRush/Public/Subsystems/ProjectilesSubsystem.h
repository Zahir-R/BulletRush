// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "../Combat/BulletBase.h"
#include "Materials/MaterialParameterCollection.h"
#include "ProjectilesSubsystem.generated.h"


UCLASS()
class BULLETRUSH_API UProjectilesSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	// Boilerplate de FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UProjectilesSubsystem, STATGROUP_Tickables); }

	// Funciones para el equipo
	ABulletBase* RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner);
	void ReturnBullet(ABulletBase* Bullet);

	float GlobalSpeedMultiplier = 1.0f; // El jefe deberá poder cambiar esto

	UMaterialParameterCollection* RhythmMPC;

	void HandleBeatHit(bool bIsStrongBeat);

	void HandleSilenceEnter();

private:
	UPROPERTY()
	TArray<ABulletBase*> BulletPool;

	int32 PoolSize = 1500;
	void InitializePool();
};
