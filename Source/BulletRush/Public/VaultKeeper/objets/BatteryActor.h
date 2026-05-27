#pragma once

#include "CoreMinimal.h"
#include "Core/Publisher.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "BatteryActor.generated.h"

class UStaticMeshComponent;
class UHealthComponent;
class ADronMecha;

UCLASS()
class BULLETRUSH_API ABatteryActor : public APublisher
{
    GENERATED_BODY()

public:
    ABatteryActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    // Vincula un drone a esta batería
    void LinkDrone(ADronMecha* Drone);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* BatteryMesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHealthComponent* HealthComp;

    UPROPERTY(EditDefaultsOnly, Category = "Battery")
    float HealRate = 20.0f;

	UBulletSpawnerComponent* BulletSpawner;

private:
    ADronMecha* LinkedDrone = nullptr;
    FTimerHandle HealLoop;

    UFUNCTION()
    void OnDeath();
    void ApplyHeal();
    void Dead();
};