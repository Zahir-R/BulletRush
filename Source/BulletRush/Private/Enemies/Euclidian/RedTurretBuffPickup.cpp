#include "Enemies/Euclidian/RedTurretBuffPickup.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Core/Euclidian/EuclidianGameMode.h"
#include "Player/PlayingPlayer.h"

ARedTurretBuffPickup::ARedTurretBuffPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere =
		CreateDefaultSubobject<USphereComponent>(
			TEXT("CollisionSphere")
		);

	RootComponent = CollisionSphere;

	CollisionSphere->SetSphereRadius(100.f);

	Mesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("Mesh")
		);

	Mesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		CubeMesh(
			TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'")
		);

	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&ARedTurretBuffPickup::OnPickup
	);
}

void ARedTurretBuffPickup::BeginPlay()
{
	Super::BeginPlay();
}

void ARedTurretBuffPickup::OnPickup(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!Cast<APlayingPlayer>(OtherActor))
	{
		return;
	}

	AEuclidianGameMode* GM =
		Cast<AEuclidianGameMode>(
			GetWorld()->GetAuthGameMode()
		);

	if (GM)
	{
		GM->EnableRedTurretVulnerability(
			10.f
		);
	}

	Destroy();
}