// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Bloodseeker/SkySphereWorld.h"
#include "Player/PlayingPlayer.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASkySphereWorld::ASkySphereWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SkyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkyMesh"));
    RootComponent = SkyMesh;

    // Mesh de esfera
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")
    );
    if (SphereMesh.Succeeded())
        SkyMesh->SetStaticMesh(SphereMesh.Object);

    // Material de nebulosa
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> SkyMat(
        TEXT("Material'/Game/Assets/nebula-skybox-16k/source/Material_001.Material_001'")
    );
    if (SkyMat.Succeeded())
        SkyMesh->SetMaterial(0, SkyMat.Object);

    ;
    SkyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SkyMesh->CastShadow = false;


}

// Called when the game starts or when spawned
void ASkySphereWorld::BeginPlay()
{
	Super::BeginPlay();
    SkyMesh->SetWorldScale3D(FVector(500.0f));
}

// Called every frame
void ASkySphereWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Player)
    {
        SetActorLocation(Player->GetActorLocation());
    }

}

