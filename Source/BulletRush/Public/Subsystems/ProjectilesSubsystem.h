#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Combat/BulletBase.h"
#include "Materials/MaterialParameterCollection.h"

#include "ProjectilesSubsystem.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UProjectilesSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
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
	float GetPlayerProjectileSpeedMultiplier(AActor* OwnerActor);

	// Funciones para el equipo
	ABulletBase* RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner, FVector Scale = FVector(0.4f));
	void ReturnBullet(ABulletBase* Bullet);

	// Return all active bullets back to pool (deactivate them)
	void ReturnAllActiveBullets();

	float GlobalSpeedMultiplier = 1.0f; // El jefe deber� poder cambiar esto

	UMaterialParameterCollection* RhythmMPC;

	void HandleBeatHit(bool bIsStrongBeat);

	void HandleSilenceEnter();

private:
	UPROPERTY()
	TArray<ABulletBase*> BulletPool;
	FDelegateHandle OnMapLoadedHandle;

	int32 PoolSize = 2500;
	void InitializePool();
};
