#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "Combat/BulletBase.h"
#include "Materials/MaterialParameterCollection.h"

#include "ProjectilesSubsystem.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UProjectilesSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool Tick(float DeltaTime);

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

	float CurrentEmissivePulse = 5.0f;
	float BaseEmissiveValue = 5.0f;
	float CurrentColorBlend = 0.0f;

	UMaterialParameterCollection* RhythmMPC;

	void HandleBeatHit(bool bIsStrongBeat, float CurrentBPM);

	void HandleSilenceEnter();

	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	void RedirectAllBossBulletsToTarget(FVector TargetLocation, float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Projectiles")
	void ExecuteSilenceCollapse(FVector TargetLocation, float CollapseSpeed);

private:
	UPROPERTY()
	TArray<ABulletBase*> BulletPool;
	FDelegateHandle OnMapLoadedHandle;
	FDelegateHandle TickHandle;

	int32 PoolSize = 2500;
	bool bIsActive = false;
	void InitializePool();
};
