#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "WeakPointComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeakPointDestroyed);

class UProjectilesSubsystem;

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class BULLETRUSH_API UWeakPointComponent : public USphereComponent
{
	GENERATED_BODY()
	
public:
	UWeakPointComponent();

    void SetVisualMaterial(UMaterialInterface* NewMat);
    
	// La batise�al que el jefe escuchar�
	UPROPERTY(BlueprintAssignable, Category = "Weak Point")
	FOnWeakPointDestroyed OnDestroyedEvent;
    
	float CurrentHealth;
    
	void TakeDamageFromHit(float DamageAmount);
    void ResetWeakPoint();
    bool IsDestroyed() const { return CurrentHealth <= 0.0f; }
    void ForceDestroy();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Weak Point")
    float MaxHealth = 100.0f;

    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crashe� sin querer----
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VisualMesh")
    class UStaticMeshComponent* VisualMesh;
    //-----------------------------------------------------------------------------------------

    UPROPERTY()
    UProjectilesSubsystem* PoolCache;

    // El punto d�bil detecta sus propias colisiones
    UFUNCTION()
    void OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

};
