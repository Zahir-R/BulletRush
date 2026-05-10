#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "Components/WeakPointComponent.h"
#include "Components/HealthComponent.h"
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
	ABossBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Boss Logic")
	EBossState CurrentState;

	int32 AttackIdentifier = 0;
	int32 ActiveWeakPoints;

	FTimerHandle IntroTimer;

	UFUNCTION()
	virtual void HandleWeakPointDestroyed();
	virtual void Die() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetBossState(EBossState NewState);

	virtual void StartAttack() override;

	void SetBossInvulnerability(bool bInv);
};
