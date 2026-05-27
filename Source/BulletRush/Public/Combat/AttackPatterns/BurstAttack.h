#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "BurstAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UBurstAttack : public UAttackStrategy
{
	GENERATED_BODY()

public:
	// Ejecuta un ataque de ráfaga, generando un número definido de balas en rápida sucesión desde el origen definido en Params.
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};