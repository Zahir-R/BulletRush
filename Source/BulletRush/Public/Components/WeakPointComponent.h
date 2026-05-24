#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "WeakPointComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeakPointDestroyed);

class UProjectilesSubsystem;

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UWeakPointComponent : public USphereComponent
{
	GENERATED_BODY()
	
public:
	UWeakPointComponent();

    void SetVisualMaterial(UMaterialInterface* NewMat);

	// La batiseñal que el jefe escuchará
	UPROPERTY(BlueprintAssignable, Category = "Weak Point")
	FOnWeakPointDestroyed OnDestroyedEvent;

    void ResetWeakPoint();
    bool IsDestroyed() const { return CurrentHealth <= 0.0f; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Weak Point")
    float MaxHealth = 100.0f;

    float CurrentHealth;

    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crasheé sin querer----
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualMesh")
    class UStaticMeshComponent* VisualMesh;
    //-----------------------------------------------------------------------------------------

    UPROPERTY()
    UProjectilesSubsystem* PoolCache;

    // El punto débil detecta sus propias colisiones
    UFUNCTION()
    void OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

};
