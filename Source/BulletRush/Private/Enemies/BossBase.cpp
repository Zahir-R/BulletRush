#include "../../Public/Enemies/BossBase.h"
#include "Engine/World.h"
#include "../../Public/Components/WeakPointComponent.h"
#include "../../Public/Components/BulletSpawnerComponent.h"

// Sets default values
ABossBase::ABossBase()
{
	HealthComp->MaxHealth = 4000.0f;
	CurrentState = EBossState::Idle;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	bAutoStartAttack = false;
	CurrentState = EBossState::Idle;
}

/**
CurrentHealth = MaxHealth;
El jefe pasa a estado Intro
Se hace un conteo de todos los puntos débiles
Despues de cinco segundos, el jefe pasa a estado idle
*/
void ABossBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Empezamos con el estado de "animación"
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
			AttackInterval = 5.0f; // Esto también debería ser definido en cada hijo, lo dejo acá por test xd
			BeginAttackLoop();
		}, 5.0f, false);
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
	
	switch (CurrentState)
	{
	case EBossState::Intro:
	case EBossState::PhaseTransition:
		SetBossInvulnerability(true);
		// Otro Timer como stunned
		break;
	case EBossState::Attacking:
		if (!GetWorldTimerManager().IsTimerActive(AttackLoopTimer))
		{
			BeginAttackLoop();
		}
		SetBossInvulnerability(true);
		break;
	case EBossState::Idle:
		UE_LOG(LogTemp, Display, TEXT("Jefe descansando..."));
		SetBossInvulnerability(true);
		break;
		// ChillFunction() o algo así, definido en cada boss

	case EBossState::Stunned:
		SetBossInvulnerability(false);
		break;
		// GetWorld()->SetTimer(TimeHandler, 5.0f, &ABossBase::SetBossState(EBossState::Idle), false) o algo así para desestunearse automaticamente xd
		// Es más conveniente utilizar algo parecido a lo que está en BeginPlay
	case EBossState::Dead:
		StopAttackLoop();
		break;
		// Poner el booleando Instance de este jefe como derrotado
		// Animación de muerte?
		// Función para salir del nivel y detruirlo :3
	}
	//
	UE_LOG(LogTemp, Warning, TEXT("Jefe cambió de estado a: %s"), *UEnum::GetValueAsString(CurrentState));

}

void ABossBase::StartAttack()
{
	// Molde de como override la función en cualquier hijo
	if (!BulletSpawner) return;
	/*
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
	*/

	TArray<FAttackStep> Combo;
	//Combo.Add(FAttackStep(EAttackType::Circle, 20, 60.0f, 0.5f, 300.0f, 10.0f));
	//FVector TestOrigin = GetActorLocation() + (GetActorRightVector() * 300.0f);
	//Combo.Add(FAttackStep(EAttackType::Spiral, 30, 50.0f, 1.0f, GetActorLocation(), 240.0f, 10.0f));
	//Combo.Add(FAttackStep(EAttackType::Burst, 5, 80.0f, 0.2f, 0.1f));

	BulletSpawner->StartSequence(Combo); // Esto debería ir en estado Attacking, lo dejo acá por test xd

}


void ABossBase::HandleWeakPointDestroyed()
{
	ActiveWeakPoints--;

	UE_LOG(LogTemp, Warning, TEXT("Punto débil destruido. Quedan: %d"), ActiveWeakPoints);

	// Si ya no quedan puntos débiles, el jefe queda expuesto
	if (ActiveWeakPoints <= 0)
	{
		SetBossInvulnerability(false);

		// Opcional: Forzarlo al estado Stunned para que el jugador aproveche
		// SetBossState(EBossState::Stunned);
	}
}

void ABossBase::Die()
{
	if (CurrentState == EBossState::Dead) return;

	SetBossState(EBossState::Dead);
	OnEnemyDeath.Broadcast(this);
	Destroy();
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

/*
float ABossBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	/*
	// Siempre llamamos a la versión del padre (AActor) por seguridad interna del motor
	float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Si somos invulnerables, estamos muertos, o el daño es negativo/cero, ignoramos todo.
	if (bIsInvulnerable || CurrentState == EBossState::Dead || RealDamage <= 0.0f)
	{
		return 0.0f;
	}

	// Hacemos un daño base
	CurrentHealth = FMath::Clamp(CurrentHealth - RealDamage, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Jefe recibió daño. Salud restante: %f"), CurrentHealth);

	if (CurrentHealth <= 0.0f)
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
	/*
	// El motor requiere que devuelvas cuánto daño se aplicó finalmente
	// Esto también se ejecutará 
	return RealDamage;
	*/
/*
	if (HealthComp) return HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return 0.0f;
}*/
/*
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
	bIsInvulnerable = newstate;
}*/
/*
void ABossBase::OnBossDeath()
{
	SetBossState(EBossState::Dead);
}*/

void ABossBase::SetBossInvulnerability(bool bInv)
{
	if (HealthComp) HealthComp->SetInvulnerable(bInv);
}