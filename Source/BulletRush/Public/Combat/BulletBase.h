#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletBase.generated.h"

USTRUCT(BlueprintType)
struct FBulletData
{
	GENERATED_BODY()

	FVector Direction;
	FVector SpawnLocation;
	float Damage;
	float Speed;
	bool bIsActive;
	bool bIsPlayerBullet;

	AActor* OwnerActor;

	// Inicializador por defecto
	FBulletData() : Direction(FVector::ZeroVector), SpawnLocation(FVector::ZeroVector), Damage(0.f), Speed(0.f), bIsActive(false), bIsPlayerBullet(false), OwnerActor(nullptr) {}
};

UCLASS(Blueprintable)
class BULLETRUSH_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	ABulletBase();

	// El Subsystem usar� esto para "despertar" la bala
	void ActivateBullet(FVector Position, FVector Direction, float Speed, bool bIsPlayerBullet, float Damage, FVector SpawnLocation, AActor* OwnerAct);

	// Para "dormir" la bala cuando choque o salga de rango
	void DesactivateBullet();

	// Datos actuales de la bala (lo que usar� el manager para moverla)
	FBulletData BulletData;

protected:
	virtual void BeginPlay() override;
	

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;
};