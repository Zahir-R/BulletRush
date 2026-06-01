#include "Test/Collectible.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PlayingPlayer.h"
#include "Core/CollectiblePickupPublisher.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACollectible::ACollectible()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->InitSphereRadius(100.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshAsset.Succeeded()) Mesh->SetStaticMesh(MeshAsset.Object);
	Mesh->SetRelativeScale3D(FVector(0.5f));
}

void ACollectible::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::OnOverlapBegin);
}

void ACollectible::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayingPlayer* Player = Cast<APlayingPlayer>(OtherActor);
	if (!Player) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectiblePickupPublisher::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		ACollectiblePickupPublisher* Pub = Cast<ACollectiblePickupPublisher>(Found[0]);
		if (Pub) Pub->RegisterPick();
	}
	Destroy();
}

void ACollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

