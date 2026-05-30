#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "SphereAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API USphereAttack : public UAttackStrategy
{
	GENERATED_BODY()

public:
	// Ejecuta un ataque esférico, generando balas distribuidas en todas las direcciones alrededor del origen definido en Params.
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};