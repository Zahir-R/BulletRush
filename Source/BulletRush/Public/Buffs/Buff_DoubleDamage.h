#pragma once

#include "CoreMinimal.h"
#include "Buffs/BuffBase.h"
#include "Buff_DoubleDamage.generated.h"

UCLASS()
class BULLETRUSH_API UBuff_DoubleDamage : public UBuffBase
{
	GENERATED_BODY()

public:
	float DamageMultiplier = 2.0f;
	virtual void Apply(AActor* InOwner) override;
	virtual void Remove() override;
};
