// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/BossBase.h"
#include "Enemies/BossState.h"
#include "Engine/World.h"
#include "Components/HealthComponent.h"
#include "Components/WeakPointComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Subsystems/ProjectilesSubsystem.h"
#include "Map/LevelPortal.h"

// Sets default values
ABossBase::ABossBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABossBase::BeginPlay()
{
	Super::BeginPlay();

	IntroState = NewObject<UBossStateIntro>(this);
	IdleState = NewObject<UBossStateIdle>(this);
	AttackingState = NewObject<UBossStateAttacking>(this);
	StunnedState = NewObject<UBossStateStunned>(this);
	PhaseTransitionState = NewObject<UBossStatePhaseTransition>(this);
	DeadState = NewObject<UBossStateDead>(this);

	Combo2.Add(FAttackStep(EAttackType::Burst, 5, 80.0f, 0.2f, 0.1f));
	Combo.Add(FAttackStep(EAttackType::Sphere, 1000, 800.0f, 0.5f, 0.1f));
}

void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentStateObject)
	{
		CurrentStateObject->UpdateState(this, DeltaTime);
	}
}

// Called to bind functionality to input
void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossBase::ChangeState(UBossState* NewState)
{
	if (!NewState) return;

	if (CurrentStateObject && CurrentStateObject != NewState)
	{
		CurrentStateObject->ExitState(this);
	}

	CurrentStateObject = NewState;
	CurrentStateObject->EnterState(this);
}

FName ABossBase::GetCurrentBossStateName() const
{
	if (CurrentStateObject)
	{
		return CurrentStateObject->GetStateTagName();
	}
	return NAME_None;
}

void ABossBase::OnTestWeakDestroyed()
{
	if (CurrentStateObject == IdleState)
	{
		ChangeState(AttackingState);
	}
}

void ABossBase::Attack()
{
	if (!BulletSpawner) return;
	switch (AttackIdentifier)
	{
	case 0:
		BulletSpawner->StartSequence(Combo);
		break;
	case 1:
		BulletSpawner->StartSequence(Combo2);
		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("Patr�n de Ataque en desarrollo"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Patr�n de Ataque Default en desarrollo"));
		break;
	}
}

void ABossBase::HandleWeakPointDestroyed()
{
	ActiveWeakPoints--;

	UE_LOG(LogTemp, Warning, TEXT("Punto d�bil destruido. Quedan: %d"), ActiveWeakPoints);

	if (ActiveWeakPoints <= 0)
	{
		ChangeState(StunnedState);
	}
}

float ABossBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	TArray<UWeakPointComponent*> WeakPoints;
	GetComponents<UWeakPointComponent>(WeakPoints);

	for (UWeakPointComponent* WP : WeakPoints)
	{
		if (WP->CurrentHealth > 0.0f)
		{
			WP->TakeDamageFromHit(DamageAmount);
			return DamageAmount;
		}
	}

	float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (RealDamage <= 0.0f) return 0.0f;

	float CurrentHP = HealthComp->CurrentHealth;
	if (CurrentHP <= 3000.0f && AttackIdentifier < 1) ChangeState(PhaseTransitionState);

	return RealDamage;
}

void ABossBase::SetInvulnerable(bool newstate)
{
	if (newstate)
	{
		// Cualquier modificación que se quiera para enemigo invencible
	}
	else
	{
		//Cualquiero otra modificación necesaria para enemigo ZoteNot
	}
	HealthComp->SetInvulnerable(newstate);
}

void ABossBase::Die()
{
	if (CurrentStateObject && Cast<UBossStateDead>(CurrentStateObject)) return;
	ChangeState(DeadState);
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		UProjectilesSubsystem* ProjSys = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
		if (ProjSys)
		{
			ProjSys->ReturnAllActiveBullets();
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform(FRotator::ZeroRotator, GetActorLocation() + FVector(300.0f, 0.0f, 50.0f));
		ALevelPortal* ReturnPortal = GetWorld()->SpawnActor<ALevelPortal>(ALevelPortal::StaticClass(), SpawnTransform, SpawnParams);
		if (ReturnPortal)
		{
			ReturnPortal->TargetLevelName = FName(TEXT("Map_CupHeadMap"));
		}
	}
	Super::Die();
}
