#include "Buffs/PowerUpBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PlayingPlayer.h"
#include "Buffs/PowerUpManager.h"
#include "Components/BuffComponent.h"

void APowerUpBase::SetManager(APowerUpManager* Manager)
{
	ManagerRef = Manager;
}

APowerUpBase::APowerUpBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetSphereRadius(64.0f);
	CollisionSphere->SetGenerateOverlapEvents(true);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APowerUpBase::OnOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (SphereMesh.Succeeded()) Mesh->SetStaticMesh(SphereMesh.Object);
	Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -64.0f));

}


void APowerUpBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;

	UBuffComponent* BuffComp = OtherActor->FindComponentByClass<UBuffComponent>();
	if (BuffComp && BuffClass) 
	{
		BuffComp->ApplyBuff(BuffClass, BuffDuration, BuffMagnitude);
		if (ManagerRef.IsValid()) ManagerRef->OnPowerUpCollected(this);
		Destroy();
	}
}