#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "CircleAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UCircleAttack : public UAttackStrategy
{
	GENERATED_BODY()

public:
	// Ejecuta un ataque circular, generando balas distribuidas en un círculo alrededor del origen definido en Params.
	virtual void Execute(class UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};