#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
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

	float RemainingLifetime = -1.f;

	FVector ConvergeCenter;
	float ConvergeDelay = -1.f;

	FBulletData() : Direction(FVector::ZeroVector), SpawnLocation(FVector::ZeroVector), Damage(0.f), Speed(0.f), bIsActive(false), bIsPlayerBullet(false), OwnerActor(nullptr), ConvergeCenter(FVector::ZeroVector), ConvergeDelay(-1.f) {}
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

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* TrailFX;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* TrailSystem;

	UPROPERTY(EditAnywhere, Category = "VFX")
	FLinearColor PlayerTrailColor = FLinearColor(0.2f, 0.8f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "VFX")
	FLinearColor EnemyTrailColor = FLinearColor(1.0f, 0.3f, 0.2f, 1.0f);

protected:
	virtual void BeginPlay() override;
	

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletMesh;
};