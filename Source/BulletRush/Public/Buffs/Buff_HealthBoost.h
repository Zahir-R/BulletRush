#pragma once

#include "CoreMinimal.h"
#include "Buffs/BuffBase.h"
#include "Buff_HealthBoost.generated.h"

UCLASS()
class BULLETRUSH_API UBuff_HealthBoost : public UBuffBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExtraHealth = 50.0f;

	virtual void Apply(AActor* InOwner) override;
	virtual void Remove() override;

};
