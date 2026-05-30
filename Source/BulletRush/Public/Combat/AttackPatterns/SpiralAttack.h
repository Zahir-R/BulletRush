#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "SpiralAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USpiralAttack : public UAttackStrategy
{
	GENERATED_BODY()

public:
	// Ejecuta un ataque en espiral, generando balas que se distribuyen en una espiral alrededor del origen definido en Params.
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};