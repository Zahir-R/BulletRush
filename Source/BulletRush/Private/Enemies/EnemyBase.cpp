// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BulletSpawnerComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshEnemy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEnemy"));
	RootComponent = MeshEnemy;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	BulletSpawner = CreateDefaultSubobject<UBulletSpawnerComponent>(TEXT("BulletSpawner"));
	

	MaxHealth = 100.0f;
	CurrentHealth = 0.0f;
	bIsInvulnerable = false;
	TeamTag = FName("Enemy");
	AtackInterval = 1.0f;
	bAutoStartAttack = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	Tags.Add(TeamTag);

	if (bAutoStartAttack && AtackInterval > 0.0f) {
		BeginAttackLoop();
	}

}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsInvulnerable || CurrentHealth <= 0.0f || RealDamage <= 0.0f) {
		return 0.0f;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth - RealDamage, 0.0f, MaxHealth);

	//UE_LOG(LogTemp, Warning, TEXT("[%s] Recibió daño.Salud: % f"), *GetName(), CurrentHealth);

	if (CurrentHealth <= 0.0f) {
		Die();
	}
	return RealDamage;
}

void AEnemyBase::SetInvulnerability(bool bNewState)
{
	bIsInvulnerable = bNewState;

}

void AEnemyBase::Die() {
	//sujeto a cambios
	StopAttackLoop();
	OnEnemyDeath.Broadcast(this);
	//UE_LOG(LogTemp, Warning, TEXT("[%s] Murió."), *GetName());
	Destroy();
}

void AEnemyBase::StartAttack()
{
	// Las clases hijas sobreescriben esto con su propia secuencia
	// Ejemplo en un hijo:
	// TArray<FAttackStep> Patron;
	// Patron.Add(FAttackStep(EAttackType::Circle, 8, 400.0f, 1.0f));
	// BulletSpawner->StartSequence(Patron);
}

void AEnemyBase::BeginAttackLoop()
{
	if (!GetWorld() || AtackInterval <= 0.0f) return;
	GetWorld()->GetTimerManager().SetTimer(
		AttackLoopTimer,
		this,
		&AEnemyBase::StartAttack,
		AtackInterval,
		true  // loop
	);
}

void AEnemyBase::StopAttackLoop()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
}


