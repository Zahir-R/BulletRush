#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedTurretBuffPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class BULLETRUSH_API ARedTurretBuffPickup : public AActor
{
	GENERATED_BODY()

public:
	ARedTurretBuffPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnPickup(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};