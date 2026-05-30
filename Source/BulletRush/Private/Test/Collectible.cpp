#include "Test/Collectible.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PlayingPlayer.h"
#include "Core/Requirements/RequirementManager.h"
#include "Core/Requirements/CollectibleRequirement.h"
#include "Core/TestGameModeBase.h"
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

	ATestGameModeBase* GM = Cast<ATestGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM && GM->RequirementManager)
	{
		for (UObject* Obj : GM->RequirementManager->SecretRequirements)
		{
			if (UCollectibleRequirement* Req = Cast<UCollectibleRequirement>(Obj))
			{
				Req->RegisterPick();
				UE_LOG(LogTemp, Warning, TEXT("Coleccionable coleccionado, Count: %d / %d"), Req->PickedCount, Req->RequiredCount);
				break; // solo uno por test
			}
		}
	}
	Destroy();
}

void ACollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

