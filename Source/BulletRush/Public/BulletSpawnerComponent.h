#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletSpawnerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UBulletSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBulletSpawnerComponent();

	void SpawnCircle(int32 BulletCount, float Speed);
	void SpawnSpiral(int32 BulletCount, float Speed, float RotationOffset);
	void SpawnBurst(int32 BulletCount, float Speed, float Interval);

private:
	float CurrentSpiralAngle = 0.0f;
	FTimerHandle BurstTimerHandle;
	int32 BulletsLeftToBurst;
	float CachedBurstSpeed;

	void ExecuteBurstStep();

protected:
	virtual void BeginPlay() override;
	
	// Falta PoolManager
	void InternalSpawn(FVector Direction, float Speed);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
