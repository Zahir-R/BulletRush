#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "WeakPointComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeakPointDestroyed);

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UWeakPointComponent : public USphereComponent
{
	GENERATED_BODY()
	
public:
	UWeakPointComponent();

	// La batiseñal que el jefe escuchará
	UPROPERTY(BlueprintAssignable, Category = "Weak Point")
	FOnWeakPointDestroyed OnDestroyedEvent;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Weak Point")
    float MaxHealth = 3000.0f;

    float CurrentHealth;


    // El punto débil detecta sus propias colisiones
    UFUNCTION()
    void OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
