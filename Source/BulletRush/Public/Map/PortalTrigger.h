#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalTrigger.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BULLETRUSH_API APortalTrigger : public AActor
{
    GENERATED_BODY()
public:
    APortalTrigger();

    // Target location inside same level
    UPROPERTY(EditAnywhere)
    FVector TeleportTarget;

    UPROPERTY(EditAnywhere)
    bool bIsActive = false;

    FSimpleMulticastDelegate OnPortalTriggered;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* TriggerSphere;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPortalOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
