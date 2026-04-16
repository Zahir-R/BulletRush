// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/Player/TopDownPlayer.h"




// Sets default values
ALevelPortal::ALevelPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	CollisionBox->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	RootComponent = CollisionBox;


	//malla  para la caja
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	// descativamos las colisiones de la malla
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));

	if (MeshAsset.Succeeded())
	{
		PortalMesh->SetStaticMesh(MeshAsset.Object);
		//ubicacion respecto al box
		PortalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	}

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelPortal::OnOverlapBegin);


}

// Called when the game starts or when spawned
void ALevelPortal::BeginPlay()
{
	Super::BeginPlay();

}
void ALevelPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Verificamos que el actor que chocó no sea nulo y no sea el portal mismo
	if (OtherActor && OtherActor != this)
	{
		// verificamos que el actor que choco sea el jugador 
		//referencia al player TopDown
		ATopDownPlayer* Player = Cast<ATopDownPlayer>(OtherActor);
		if (Player && !TargetLevelName.IsNone())
		{
			//cargamos el nivel corespondiente
			UGameplayStatics::OpenLevel(this, TargetLevelName);
		}
	}
}

// Called every frame
void ALevelPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

