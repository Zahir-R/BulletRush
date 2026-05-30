#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;

	float MaxHealth;
	float CurrentHealth;
	bool bIsInvulnerable;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	FName TeamTag;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackInterval;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutoStartAttack;

	FTimerHandle AttackLoopTimer;

	UFUNCTION()
	virtual void OnHealthDeath();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetInvulnerability(bool bNewState);

	void BeginAttackLoop();
	void StopAttackLoop();
	
	virtual void StartAttack();
	virtual void Die();
	UFUNCTION()
	virtual void Attack();
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshEnemy;
	UPROPERTY(VisibleAnywhere)
	UShapeComponent* Hitbox;
	UPROPERTY(VisibleAnywhere)
	UBulletSpawnerComponent* BulletSpawner;
	
	FOnEnemyDeath OnEnemyDeath;
};
