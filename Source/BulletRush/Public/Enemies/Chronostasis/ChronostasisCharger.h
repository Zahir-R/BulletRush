#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Components/SphereComponent.h"
#include "ChronostasisCharger.generated.h"

class UMovementStrat;

UCLASS(Blueprintable)
class BULLETRUSH_API AChronostasisCharger : public AEnemyBase
{
	GENERATED_BODY()
public:
	AChronostasisCharger();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY()
	UMovementStrat* MovementStrategy;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* HitCollision;

	UFUNCTION()
	void OnHitPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
