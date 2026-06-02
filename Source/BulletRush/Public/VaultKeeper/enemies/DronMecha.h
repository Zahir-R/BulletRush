#pragma once

#include "CoreMinimal.h"
#include "Enemies/Common/Drone.h"
#include "Core/Subscriber.h"
#include "DronMecha.generated.h"

class ABatteryActor;

UENUM()
enum class EDroneState : uint8
{
    Open,
    Closed
};

UCLASS()
class BULLETRUSH_API ADronMecha : public ADrone, public ISubscriber
{
    GENERATED_BODY()

public:
    ADronMecha();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

public:
    // --- Estado ---
    UPROPERTY(VisibleAnywhere, Category = "DronMecha|State")
    EDroneState CurrentDroneState;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Visual")
    UMaterialInterface* OpenMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Visual")
    UMaterialInterface* ClosedMaterial;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Cycle")
    float OpenDuration = 14.0f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Cycle")
    float ClosedDuration = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Movement")
    float RotationSpeed = 5.0f;

    ABatteryActor* LinkedBattery = nullptr;

    void LinkBattery(ABatteryActor* Battery);

    // ISubscriber
    virtual void Update(APublisher* Publisher) override;

    // Override TakeDamage — solo recibe daño si está abierto
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void StartAttack() override;

private:
    FTimerHandle CycleTimer;

    void Open();
    void Close();
    void SetDroneState(EDroneState NewState);
    void RotateTowardsPlayer(float DeltaSeconds);

    FVector HomeLocation;
    float OrbitAngle = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Movement")
    float OrbitRadius = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Movement")
    float OrbitSpeed = 5.2f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Movement")
    float HoverAmplitude = 15.0f;

    UPROPERTY(EditDefaultsOnly, Category = "DronMecha|Movement")
    float HoverFrequency = 2.0f;
};
