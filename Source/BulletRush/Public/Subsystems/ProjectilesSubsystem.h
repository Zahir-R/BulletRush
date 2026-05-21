// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "../Combat/BulletBase.h"
#include "../Core/Observer.h"
#include "Materials/MaterialParameterCollection.h"
#include "ProjectilesSubsystem.generated.h"

class ABossBase;

UCLASS()
class BULLETRUSH_API UProjectilesSubsystem : public UGameInstanceSubsystem, public FTickableGameObject, public IObserver
{
	GENERATED_BODY()

public:
	// Boilerplate de FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return true; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UProjectilesSubsystem, STATGROUP_Tickables); }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ClearPool();
	void ReinitializePool();


	// Funciones para el equipo
	ABulletBase* RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner);
	void ReturnBullet(ABulletBase* Bullet);

	float GlobalSpeedMultiplier = 1.0f; // El jefe deberá poder cambiar esto

	UMaterialParameterCollection* RhythmMPC;

	void HandleBeatHit(bool bIsStrongBeat);

	void HandleSilenceEnter();

	virtual void Update(class APublisher* Publisher) override;
private:
	UPROPERTY()
	TArray<ABulletBase*> BulletPool;
	FDelegateHandle OnMapLoadedHandle;

	int32 PoolSize = 5000;
	void InitializePool();

public:
	UPROPERTY()
	ABossBase* Boss;
};
