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

	// Bullet uses QueryOnly collision with Overlap responses so that
	// WeakPointComponent can detect overlaps via OnComponentBeginOverlap.
	BulletMesh->SetSimulatePhysics(false);
	BulletMesh->SetEnableGravity(false);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BulletMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	BulletMesh->SetGenerateOverlapEvents(true);
	
	TrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFX"));
	TrailFX->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> TrailAsset(
		TEXT("NiagaraSystem'/Game/MixedVFX/Particles/Projectiles/NS_GalaxyArrow.NS_GalaxyArrow'"));
	if (TrailAsset.Succeeded()) TrailSystem = TrailAsset.Object;
	TrailFX->bAutoActivate = false;

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

	BulletData.RemainingLifetime = -1.f;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	if (TrailFX && TrailSystem)
	{
		TrailFX->SetAsset(TrailSystem);
		FLinearColor Color = bIsPlayerBullet ? PlayerTrailColor : EnemyTrailColor;
		TrailFX->SetColorParameter(FName("Color"), Color);
		TrailFX->SetColorParameter(FName("User.Color"), Color);
		TrailFX->Activate(true);
	}
}
void ABulletBase::DesactivateBullet()
{
	BulletData.bIsActive = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorRelativeScale3D(FVector(0.4f));
	// La mandamos lejos para que no estorbe
	SetActorLocation(FVector(0, 0, -5000.f));
	if (TrailFX)
	{
		TrailFX->Deactivate();
	}
}

