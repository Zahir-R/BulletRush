// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Enemies/BossBase.h"
#include "Engine/World.h"
#include "Components/HealthComponent.h"
#include "../../Public/Components/WeakPointComponent.h"
#include "../../Public/Components/BulletSpawnerComponent.h"

// Sets default values
ABossBase::ABossBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentState = EBossState::Idle;

	bAutoStartAttack = false;
	//bIsInvulnerable = false;
	HealthComp->MaxHealth = 4000.0f;

	BulletSpawner = CreateDefaultSubobject<UBulletSpawnerComponent>(TEXT("BulletSpawnerr"));


	Tags.Add("Jefe");

	TestWeak = CreateDefaultSubobject<UWeakPointComponent>(TEXT("TestWeakPoint"));
	TestWeak->SetupAttachment(RootComponent);
	TestWeak->SetRelativeLocation(FVector(-50.0f, -250.0f, 100.0f));

	//bIsInvulnerable = true;
	HealthComp->SetInvulnerable(true);
	//BossMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossMesh"));
	//RootComponent = BossMesh;

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	//if (MeshAsset.Succeeded()) BossMesh->SetStaticMesh(MeshAsset.Object);
	//Hitbox->SetGenerateOverlapEvents(true);
}

/**
CurrentHealth = MaxHealth;
El jefe pasa a estado Intro
Se hace un conteo de todos los puntos débiles
Despues de cinco segundos, el jefe pasa a estado idle
*/
void ABossBase::BeginPlay()
{
	//APawn::BeginPlay();
	//HealthComp->CurrentHealth = 5000.0f;
	// Empezamos con el estado de "animación"

	Super::BeginPlay();

	HealthComp->CurrentHealth = 5000.0f;

	//HealthComp->OnHealthChanged.AddDynamic(this, &ABossBase::OnHealthChangedHandler);
	//HealthComp->OnDeath.AddDynamic(this, &ABossBase::OnBossDeath);


	SetBossState(EBossState::Intro);
	ActiveWeakPoints = 0;

	// Buscamos automáticamente cuántos puntos débiles tiene este jefe (Deben estar definidos en el constructor del hijo)
	TArray<UWeakPointComponent*> WeakPoints;
	GetComponents<UWeakPointComponent>(WeakPoints);

	// Los contamos y quedamos atentos a la batiseñal
	for (UWeakPointComponent* WP : WeakPoints)
	{
		ActiveWeakPoints++;
		WP->OnDestroyedEvent.AddDynamic(this, &ABossBase::HandleWeakPointDestroyed);
	}
	// El jefe es invulnerable 5 segundos, luego de cinco segundos se queda en estado Idle(no hace nada o se mueve a algún lado xd)
	GetWorld()->GetTimerManager().SetTimer(IntroTimer, [this]()
		{
			SetBossState(EBossState::Idle); 
		}, 2.0f, false);

	//Combo.Add(FAttackStep(EAttackType::Circle, 20, 60.0f, 0.5f, 300.0f, 10.0f));
	//FVector TestOrigin = GetActorLocation() + (GetActorRightVector() * 300.0f);
	//Combo.Add(FAttackStep(EAttackType::Spiral, 30, 50.0f, 1.0f, GetActorLocation(), 240.0f, 10.0f));
	Combo2.Add(FAttackStep(EAttackType::Burst, 5, 80.0f, 0.2f, 0.1f));
	Combo.Add(FAttackStep(EAttackType::Sphere, 1000, 800.0f, 0.5f, 0.1f));
}

/**
Called every frame
Os dejo la función totalmente libre
*/
void ABossBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABossBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/**
Aquí defines que hace cada jefe en su cambio de estado
Puedes apodarlo centro de mando si gustas xD
*/
void ABossBase::SetBossState(EBossState NewState)
{
	// Molde de como override la función en cualquier hijo
	CurrentState = NewState;
	// Zona de Testo!!
	switch (CurrentState)
	{
	case EBossState::Attacking:
		HealthComp->SetInvulnerable(false);
		Attack();
		GetWorld()->GetTimerManager().SetTimer(AttackLoopTimer, this, &ABossBase::Attack, 3.0f, true); // Testing, esto debería ser en Attacking xd
		break;
	case EBossState::Stunned:
		HealthComp->SetInvulnerable(false);
		UE_LOG(LogTemp, Display, TEXT("Jefe Vulnerable!!"));
		GetWorld()->GetTimerManager().SetTimer(StunnedTimer, [this]()
			{
				GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer); // Detenemos el ataque anterior
				SetBossState(EBossState::Attacking);
			}, 5.0f, false);
		break;
	case EBossState::Idle:
		UE_LOG(LogTemp, Display, TEXT("Jefe descansando..."));
		break;
		// ChillFunction() o algo así, definido en cada boss
	case EBossState::Intro:
		HealthComp->SetInvulnerable(true);
		break;
		// Animación de intro();
	case EBossState::PhaseTransition:
		HealthComp->SetInvulnerable(true);
		UE_LOG(LogTemp, Warning, TEXT("Jefe en transición de fase"));

		if (!PhaseTransitionTimer.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(PhaseTransitionTimer);
		}

		if (GetWorld())
		{
			AttackIdentifier++;
			GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer); // Detenemos el ataque anterior
			GetWorld()->GetTimerManager().SetTimer(PhaseTransitionTimer, [this]()
				{
					SetBossState(EBossState::Attacking);
				}, 5.0f, false);
		}
		// Otro Timer como stunned
		break;

	case EBossState::Dead:
		break;
		// Poner el booleando Instance de este jefe como derrotado
		// Animación de muerte?
		// Función para salir del nivel y detruirlo :3
	};
	//----------------

}

void ABossBase::Attack()
{
	// Molde de como override la función en cualquier hijo
	if (!BulletSpawner) return;
	// Zona de Testeo!!!
	switch (AttackIdentifier) // Identificador de ataques, los hijos también lo heredan
	{
	case 0:
		// PrimerPatronDeAtaque();
		BulletSpawner->StartSequence(Combo);
		break;
	case 1:
		// Codigo de primer patrón de ataque
		BulletSpawner->StartSequence(Combo2);
		break;
	case 2:
		// Codigo de segundo patrón de ataque
		UE_LOG(LogTemp, Warning, TEXT("Patrón de Ataque en desarrollo"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Patrón de Ataque Default en desarrollo"));
		break;
		// DefaultAttack();
	}
	//-------------------

	
	

}


void ABossBase::HandleWeakPointDestroyed()
{
	ActiveWeakPoints--;

	UE_LOG(LogTemp, Warning, TEXT("Punto débil destruido. Quedan: %d"), ActiveWeakPoints);

	// Si ya no quedan puntos débiles, el jefe queda expuesto
	if (ActiveWeakPoints <= 0)
	{
		SetBossState(EBossState::Stunned);

		// Opcional: Forzarlo al estado Stunned para que el jugador aproveche
		// SetBossState(EBossState::Stunned);
	}
}

/**
Función integrada de Unreal para que el Jefe reciba daño
Esta función debería ser la única (aparte de BeginPlay() o excepciones) que maneje el SetBossState()
Tick no debería manejar ningún SetBossState
-------
La función verifica si Boss es invulnerable, esta muero o el daño es negativo o cero, en ese caso no hace nada de daño
Luego resta el daño hecho de la vida actual del jefe, si la vida del jefe baja de cero, Setea al jefe en muerto
El resto se define en clases hijas
*/
float ABossBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (RealDamage <= 0.0f) return 0.0f;

	float CurrentHP = HealthComp->CurrentHealth;
	if (CurrentHP <= 3000.0f && AttackIdentifier < 1) SetBossState(EBossState::PhaseTransition);

	return RealDamage;

	/*
	
	
	// Siempre llamamos a la versión del padre (AActor) por seguridad interna del motor
	float RealDamage = APawn::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Si somos invulnerables, estamos muertos, o el daño es negativo/cero, ignoramos todo.
	if (bIsInvulnerable || CurrentState == EBossState::Dead || RealDamage <= 0.0f)
	{
		return 0.0f;
	}

	// Hacemos un daño base
	CurrentHealthh = FMath::Clamp(CurrentHealthh - RealDamage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Jefe recibe danio. Salud restante: %f"), CurrentHealthh);

	
	if (CurrentHealthh > 0.0f && CurrentHealthh <= 3000.0f && AttackIdentifier < 1)
	{
		SetBossState(EBossState::PhaseTransition);
	}
	else if (CurrentHealthh <= 0.0f)
	{
		SetBossState(EBossState::Dead);
	}
	

	// Esto queda a su necesidad cuando hereden de esta clase, todo lo que está arriba se ejecutará antes de lo que 
	// lleguen a escribir luego (En teoría xD, no pude comprobarlo)
	/*
	
	// Ejemplo: Si la salud baja del 50% y seguimos en la fase 0, cambiamos de fase
	if (CurrentHealth <= MaxHealth * 0.5f && CurrentPhaseIndex == 0) // O EBossPhase::primera, depende de ustedes weyes
	{
		// SetBossState se encargará de hacer invulnerable al jefe y reproducir animaciones
		SetBossState(EBossState::PhaseTransition);
	}
	*/
	// El motor requiere que devuelvas cuánto daño se aplicó finalmente
	// Esto también se ejecutará 
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
	//bIsInvulnerable = newstate;
	HealthComp->SetInvulnerable(newstate);
}

void ABossBase::Die()
{
	if (CurrentState == EBossState::Dead) return;
	SetBossState(EBossState::Dead);
	NotifySubscribers();
	Super::Die();
}