#pragma once
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "BuffComponent.generated.h"

USTRUCT()
struct FActiveDecorator
{
    GENERATED_BODY()

    UPROPERTY()
    FTimerHandle Timer;

    UPROPERTY()
    UPlayerStatsDecorator* Decorator = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLETRUSH_API UBuffComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UBuffComponent();

    // Original API used by power-ups: apply a decorator class with duration and magnitude
    void ApplyBuff(TSubclassOf<UPlayerStatsDecorator> DecoratorClass, float Duration, float Magnitude);

    // New helper: apply and return the created decorator instance (useful for manual removal)
    UPlayerStatsDecorator* ApplyBuffAndReturn(TSubclassOf<UPlayerStatsDecorator> DecoratorClass, float Duration, float Magnitude);

    void RemoveDecorator(UPlayerStatsDecorator* Decorator);

    // Remove any decorator instances matching the provided class
    void RemoveDecoratorByClass(TSubclassOf<UPlayerStatsDecorator> DecoratorClass);

    // Check whether any decorator of the given class is active
    bool HasDecoratorOfClass(TSubclassOf<UPlayerStatsDecorator> DecoratorClass) const;

    void ClearAllBuffs();

protected:
    UPROPERTY()
    TArray<FActiveDecorator> ActiveDecorators;
};
