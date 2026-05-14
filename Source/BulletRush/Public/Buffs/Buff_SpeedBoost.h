#pragma once

#include "CoreMinimal.h"
#include "Buffs/BuffBase.h"
#include "Buff_SpeedBoost.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UBuff_SpeedBoost : public UBuffBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 2.0f;
	virtual void Apply(AActor* InOwner) override;
	virtual void Remove() override;

private:
	float OriginalSpeed;
};