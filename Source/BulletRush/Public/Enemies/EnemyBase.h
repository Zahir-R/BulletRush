// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "EnemyBase.generated.h"

class UShapeComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeath, AEnemyBase*, DeadEnemy);

UCLASS()
class BULLETRUSH_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float MaxHealth;
	float CurrentHealth;
	FName TeamTag;
	bool bIsInvulnerable;
	float AtackInterval;
	bool bAutoStartAttack;

	FTimerHandle AttackLoopTimer;

	virtual void StartAttack();
	virtual void Die();

public:	
	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetInvulnerability(bool bNewState);

	void BeginAttackLoop();
	void StopAttackLoop();
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshEnemy;
	UPROPERTY(VisibleAnywhere);
	UShapeComponent* Hitbox;
	UPROPERTY(VisibleAnywhere)
	UBulletSpawnerComponent* BulletSpawner;
	
	FOnEnemyDeath OnEnemyDeath;



};
