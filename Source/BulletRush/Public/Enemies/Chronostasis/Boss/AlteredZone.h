#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "AlteredZone.generated.h"

class ABulletBase;

UCLASS(Blueprintable)
class BULLETRUSH_API AAlteredZone : public AActor
{
	GENERATED_BODY()

public:
	AAlteredZone();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnLifetimeExpired();

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, Category = "Zone")
	float ZoneRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Zone")
	float ZoneLifetime = 20.f;

	UPROPERTY(EditAnywhere, Category = "Zone")
	float SlowSpeed = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Zone")
	float SpeedUp = 300.0f;

	TMap<ABulletBase*, float> OriginalSpeeds;

	FTimerHandle LifetimeTimerHandle;
};
