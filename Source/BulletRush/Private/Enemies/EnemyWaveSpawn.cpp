#include "Enemies/EnemyWaveSpawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Enemies/EnemyBase.h"
#include "Engine/World.h"

// Sets default values
AEnemyWaveSpawn::AEnemyWaveSpawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("Mesh")
	);

	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		CubeMesh(
			TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'")
		);

	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void AEnemyWaveSpawn::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("Spawner");
}


void AEnemyWaveSpawn::SpawnWave()
{
	ActiveEnemies.Empty();

	for (int32 i = 0; i < 2; i++)
	{
		FVector SpawnLocation =
			GetActorLocation() +
			FVector(300.f + i * 300.f, 0.f, 0.f);

		AEnemyBase* Enemy =
			GetWorld()->SpawnActor<AEnemyBase>(
				EnemyClass,
				SpawnLocation,
				FRotator::ZeroRotator
			);

		if (Enemy)
		{
			Enemy->OnEnemyDeath.AddDynamic(
				this,
				&AEnemyWaveSpawn::OnEnemyKilled
			);

			ActiveEnemies.Add(Enemy);
		}
	}
}
void AEnemyWaveSpawn::OnEnemyKilled(
	AEnemyBase* DeadEnemy)
{
	ActiveEnemies.Remove(DeadEnemy);

	if (ActiveEnemies.Num() == 0)
	{
		SpawnWave();
	}
}