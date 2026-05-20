#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "BuffComponent.generated.h"

class UPlayerStatsDecorator;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETRUSH_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();

	UFUNCTION(BlueprintCallable)
	void ApplyBuff(TSubclassOf<UPlayerStatsDecorator> DecoratorClass, float Duration, float Magnitude);

	UFUNCTION(BlueprintCallable)
	void ClearAllBuffs();

protected:
	UPROPERTY()
	TArray<UPlayerStatsDecorator*> ActiveBuffs;

private:
	struct FActiveDecorator
	{
		UPlayerStatsDecorator* Decorator;
		FTimerHandle Timer;
	};
	TArray<FActiveDecorator> ActiveDecorators;
	void RemoveDecorator(UPlayerStatsDecorator* Decorator);
};
