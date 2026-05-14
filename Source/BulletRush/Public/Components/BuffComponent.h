#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class UBuffBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();

	UFUNCTION(BlueprintCallable)
	void ApplyBuff(TSubclassOf<UBuffBase> BuffClass, float Duration, float Magnitude);

	UFUNCTION(BlueprintCallable)
	void ClearAllBuffs();

protected:
	UPROPERTY()
	TArray<UBuffBase*> ActiveBuffs;
		
};
