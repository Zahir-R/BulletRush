#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "SurroundingBulletsAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USurroundingBulletsAttack : public UAttackStrategy
{
	GENERATED_BODY()

public:
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};
