// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletBase.h"

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
	DesactivateBullet();
	

}

// Called when the game starts or when spawned
void ABulletBase::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABulletBase::ActivateBullet(FVector position, FVector Direction, float Speed, bool bIsPlayerBullet,float Damage, FVector SpawnLocation )
{
	SetActorLocation(position);
	BulletData.Direction = Direction;
	BulletData.Speed = Speed;
	BulletData.bIsActive = true;
	BulletData.bIsPlayerBullet = bIsPlayerBullet;
	BulletData.Damage = Damage;
	BulletData.SpawnLocation = SpawnLocation;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
void ABulletBase::DesactivateBullet()
{
	BulletData.bIsActive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	// La mandamos lejos para que no estorbe
	SetActorLocation(FVector(0, 0, -5000.f));
}

