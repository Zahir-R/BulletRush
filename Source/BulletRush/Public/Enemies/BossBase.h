// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.h"
#include "Components/StaticMeshComponent.h"
#include "BossBase.generated.h"

class UShapeComponent;
class UBulletSpawnerComponent;

UENUM(BluePrintType)
enum class EBossState : uint8
{
	// Jefe invulnerable, ejecuta "animación" de entrada
	Intro UMETA(DisplayName = "Introduccion"),
	Attacking UMETA(DisplayName = "Ejecutando Patrón"),
	Idle UMETA(DisplayName = "Moviendose / Esperando"),
	Stunned UMETA(DisplayName = "Indefenso"),
	PhaseTransition UMETA(DisplayName = "Cambio de Fase"),
	Dead UMETA(DisplayName = "Derrotado/Muerto")
};

UCLASS()
class BULLETRUSH_API ABossBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABossBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Boss Logic")
	EBossState CurrentState;

	UPROPERTY(VisibleAnywhere, Category = "Boss Logic")
	bool bIsInvulnerable;

	int32 AttackIdentifier = 0; // Define que patrón de ataque utilizar

	// Cuántos puntos débiles le quedan vivos
    int32 ActiveWeakPoints;

    // Función que se ejecutará cuando un punto débil grite
    UFUNCTION()
    virtual void HandleWeakPointDestroyed();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetBossState(EBossState NewState);

	virtual void Attack();

	// Mesh para la nave, cada hijo puede usarla
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BossMesh;

	// La HitBox se usará en caso de que la mesh no tenga colisiones bien definidas
	//UPROPERTY(VisibleAnywhere, Category = "Components")
	//UShapeComponent* Hitbox; // Hitbox estática, no se como aplicar la hitbox para un gusano de multiples partes :)

	float MaxHealth = 4000.0f;
	float CurrentHealthh = 4000.0f;

	FTimerHandle IntroTimer;

	void SetInvulnerable(bool newstate);

	UBulletSpawnerComponent* BulletSpawner;
	FTimerHandle AttackLoopTimer;

};
