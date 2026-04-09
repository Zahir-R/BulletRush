// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Enemies/BossBase.h"

// Sets default values
ABossBase::ABossBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossBase::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	
}

// Called every frame
void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Si condición especial de tu jefe (CurrentHealth <= Umbral) por ejemplo
	// SetBossState(ElEstadoQueQuieras)

}

// Called to bind functionality to input
void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossBase::SetBossState(EBossState NewState)
{
	// Molde de como override la función en cualquier hijo
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EBossState::Attacking:
		Attack();
		break;
	case EBossState::Stunned:
		bIsInvulnerable = false;
		break;
		// GetWorld()->SetTimer(TimeHandler, 5.0f, &ABossBase::SetBossState(EBossState::Idle), false) o algo así para desestunearse automaticamente xd
	case EBossState::Idle:
		UE_LOG(LogTemp, Display, TEXT("Jefe descansando..."));
		break;
		// ChillFunction() o algo así, definido en cada boss
	case EBossState::Intro:
		break;
		// Animación de intro();
	case EBossState::PhaseTransition:
		bIsInvulnerable = true;
		// Otro Timer como stunned
		break;

	case EBossState::Dead:
		break;
		// Poner el booleando Instance de este jefe como derrotado
		// Animación de muerte?
		// Función para salir del nivel y detruirlo :3
	};
	//

}

void ABossBase::Attack()
{
	// Molde de como override la función en cualquier hijo
	switch (AttackIdentifier) // Identificador de ataques, los hijos también lo heredan
	{
	case 0:
		// PrimerPatronDeAtaque();
		break;
	case 1:
		// Codigo de primer patrón de ataque
		break;
	case 2:
		// Codigo de segundo patrón de ataque
		break;
	default:
		break;
		// DefaultAttack();
	}
}
