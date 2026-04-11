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

	// Inicializador por defecto
	FBulletData() : Direction(FVector::ZeroVector), Speed(0.f), bIsActive(false), bIsPlayerBullet(false), Damage(0.f), SpawnLocation(FVector::ZeroVector) {}
};

UCLASS()
class BULLETRUSH_API ABulletBase : public AActor
{
	GENERATED_BODY()

public:
	ABulletBase();

	// El Subsystem usará esto para "despertar" la bala
	void ActivateBullet(FVector Position, FVector Direction, float Speed, bool bIsPlayerBullet, float Damage, FVector SpawnLocation);

	// Para "dormir" la bala cuando choque o salga de rango
	void DesactivateBullet();

	// Datos actuales de la bala (lo que usará el manager para moverla)
	FBulletData BulletData;

protected:
	virtual void BeginPlay() override;
	

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;
};