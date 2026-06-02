#include "Enemies/EnemyBase.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*
	MeshEnemy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEnemy"));
	RootComponent = MeshEnemy;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));

	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
	*/
	BulletSpawner = CreateDefaultSubobject<UBulletSpawnerComponent>(TEXT("BulletSpawner"));
	
	

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->MaxHealth = 100.0f;

	TeamTag = FName("Enemy");
	AttackInterval = 1.0f;
	bAutoStartAttack = true;

	Tags.Add("Enemigo");
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	bIsInvulnerable = false;

	HealthComp->CurrentHealth = HealthComp->MaxHealth;
	Tags.Add(TeamTag);

	HealthComp->OnDeath.AddDynamic(this, &AEnemyBase::OnHealthDeath);

	if (bAutoStartAttack && AttackInterval > 0.0f) {
		BeginAttackLoop();
	}


}

float AEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	if (HealthComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Recibi� da�o: %f"), *GetName(), DamageAmount);
		return HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	}
	return 0.0f;
}

void AEnemyBase::SetInvulnerability(bool bNewState)
{
	if (HealthComp)
	{
		HealthComp->SetInvulnerable(bNewState);
	}
}

void AEnemyBase::OnHealthDeath()
{
	Die();
}

void AEnemyBase::Die() {
	if (bIsDead) return;
	bIsDead = true;
	StopAttackLoop();
	OnEnemyDeath.Broadcast(this);
	// UE_LOG(LogTemp, Warning, TEXT("[%s] Muri�."), *GetName());
	Destroy();
}

void AEnemyBase::StartAttack()
{
	// Las clases hijas sobreescriben esto con su propia secuencia
	// Ejemplo en un hijo:
	// TArray<FAttackStep> Patron;
	// Patron.Add(FAttackStep(EAttackType::Circle, 8, 400.0f, 1.0f));
	// BulletSpawner->StartSequence(Patron);
	// 1. Verificamos que el spawner exista
	//test:
	if (!BulletSpawner) return;
	FVector FireDirection = GetActorForwardVector();
	TArray<FAttackStep> TestCombo;
	TestCombo.Add(FAttackStep());
	BulletSpawner->StartSequence(TestCombo);

//	UE_LOG(LogTemp, Log, TEXT("[%s] Ejecutando disparo de prueba."), *GetName());


}

FVector AEnemyBase::ApplyEnemySeparation(const FVector& DesiredLocation) const
{
	FVector Adjusted = DesiredLocation;
	UWorld* World = GetWorld();
	if (!World) return Adjusted;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, AEnemyBase::StaticClass(), Enemies);

	for (AActor* Other : Enemies)
	{
		if (Other == this || !Other || !IsValid(Other)) continue;
		FVector Delta = Adjusted - Other->GetActorLocation();
		float Dist = Delta.Size();
		if (Dist < SeparationDistance && Dist > KINDA_SMALL_NUMBER)
		{
			FVector PushDir = Delta.GetSafeNormal();
			float PushAmount = (SeparationDistance - Dist) * 0.5f;
			Adjusted += PushDir * PushAmount;
		}
	}
	return Adjusted;
}

void AEnemyBase::BeginAttackLoop()
{
	if (!GetWorld() || AttackInterval <= 0.0f) return;
	GetWorld()->GetTimerManager().SetTimer(
		AttackLoopTimer,
		this,
		&AEnemyBase::StartAttack,
		AttackInterval,
		true  // loop
	);
}

void AEnemyBase::StopAttackLoop()
{
	if (GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
}


