#pragma once

#include "CoreMinimal.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "FanAttack.generated.h"

UCLASS(Blueprintable)
class BULLETRUSH_API UFanAttack : public UAttackStrategy
{
    GENERATED_BODY()

public:
    virtual void Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params) override;
};
