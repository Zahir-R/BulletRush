#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BuffBase.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UBuffBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration;

	UPROPERTY()
	AActor* Owner;

	virtual void Apply(AActor* InOwner);
	virtual void Remove();
	
	void StartDurationTimer();

protected:
	FTimerHandle DurationTimer;
};
