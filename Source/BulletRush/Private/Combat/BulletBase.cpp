// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Combat/BulletBase.h"

// Sets default values
ABulletBase::ABulletBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	RootComponent = BulletMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletMeshAsset(TEXT("/Engine/BasicShapes/Sphere"));
	if (BulletMeshAsset.Succeeded()) {

		BulletMesh->SetStaticMesh(BulletMeshAsset.Object);
		BulletMesh->SetWorldScale3D(FVector(0.4f)); // Escalamos la esfera para que parezca una bala
	}

	// Bullets should not simulate physics or block other actors. Movement and
	// collision detection is handled by the ProjectilesSubsystem using traces.
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetEnableGravity(false);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 BulletMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	DesactivateBullet();
	

}

// Called when the game starts or when spawned
void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABulletBase::ActivateBullet(FVector position, FVector Direction, float Speed, bool bIsPlayerBullet,float Damage, FVector SpawnLocation, AActor* OwnerAct)
{
	SetActorLocation(position);
	BulletData.Direction = Direction;
	BulletData.Speed = Speed;
	BulletData.bIsActive = true;
	BulletData.bIsPlayerBullet = bIsPlayerBullet;
	BulletData.Damage = Damage;
	BulletData.SpawnLocation = SpawnLocation;
	BulletData.OwnerActor = OwnerAct;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
void ABulletBase::DesactivateBullet()
{
	BulletData.bIsActive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorRelativeScale3D(FVector(0.4f));
	// La mandamos lejos para que no estorbe
	SetActorLocation(FVector(0, 0, -5000.f));
}

