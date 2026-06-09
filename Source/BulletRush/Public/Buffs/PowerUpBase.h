#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStatsDecorator.h"
#include "PowerUpBase.generated.h"

class APowerUpManager;

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
	Health,
	Speed,
	Damage,
	Ultimate,
};

UCLASS(Blueprintable)
class BULLETRUSH_API APowerUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerUpBase();

	void SetManager(APowerUpManager* Manager);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPowerUpType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UPlayerStatsDecorator> BuffClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuffDuration = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuffMagnitude = 1.0f;

	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	TWeakObjectPtr<APowerUpManager> ManagerRef;

};
