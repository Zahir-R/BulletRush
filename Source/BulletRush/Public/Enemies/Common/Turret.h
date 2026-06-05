#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Turret.generated.h"

UCLASS()
class BULLETRUSH_API ATurret : public AEnemyBase
{
	GENERATED_BODY()

public:

	ATurret();
	virtual void BeginPlay() override;

protected:

	virtual void Tick(float DeltaTime) override;
	virtual void StartAttack() override;

	UPROPERTY(EditAnywhere)
	float DetectionRange = 1500.f;

	UPROPERTY(EditAnywhere)
	float ProjectileSpeed = 1500.f;

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
};