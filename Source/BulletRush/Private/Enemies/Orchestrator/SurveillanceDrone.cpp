// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/SurveillanceDrone.h"
#include "Components/StealthVisionComponent.h"
#include "Components/HealthComponent.h"
#include "Enemies/Orchestrator/SurveillanceAIControler.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

ASurveillanceDrone::ASurveillanceDrone()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(50.0f);
	CollisionComp->SetMobility(EComponentMobility::Movable);

	CollisionComp->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComp->SetCanEverAffectNavigation(false);

	// Protección de Jerarquía
	if (RootComponent)
	{
		CollisionComp->SetupAttachment(RootComponent);
	}
	else
	{
		RootComponent = CollisionComp;
	}

	// 2. ADJUNTAMOS LA MALLA A LA ESFERA
	MeshEnemy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEnemy"));
	MeshEnemy->SetupAttachment(CollisionComp); // <-- Asegúrate de adjuntarlo al CollisionComp
	MeshEnemy->SetMobility(EComponentMobility::Movable);
	MeshEnemy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshEnemy->SetSimulatePhysics(false);
	MeshEnemy->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	VisionComp = CreateDefaultSubobject<UStealthVisionComponent>(TEXT("VisionComp"));

	// 3. MOVIMIENTO: AHORA EMPUJAMOS LA ESFERA
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = CollisionComp; // <-- CRÍTICO
	MovementComp->MaxSpeed = 250.0f;
	MovementComp->NavAgentProps.bCanWalk = true;
	MovementComp->NavAgentProps.bCanFly = true;
	bUseControllerRotationYaw = true;

	// Inicializamos la malla del cono
	VisionConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionConeMesh"));
	VisionConeMesh->SetCanEverAffectNavigation(false);
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));
	if (ConeMesh.Succeeded())
	{
		VisionConeMesh->SetStaticMesh(ConeMesh.Object);
		VisionConeMesh->SetupAttachment(MeshEnemy);
		VisionConeMesh->SetMobility(EComponentMobility::Movable);
		// CRÍTICO: Desactivamos las colisiones del cono para que no bloquee proyectiles ni empuje al jugador
		VisionConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisionConeMesh->SetCastShadow(false); // Evita sombras raras en áreas de sigilo
		VisionConeMesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}

	

	// Desactivamos el comportamiento de ataque base
	bAutoStartAttack = false;
	AttackInterval = 0.0f;

	if (HealthComp)
	{
		HealthComp->MaxHealth = 10.0f; // Frágil para recompensar precisión
	}

	// Asignamos el AIController que creamos en el paso anterior
	AIControllerClass = ASurveillanceAIControler::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASurveillanceDrone::BeginPlay()
{
	Super::BeginPlay();

	if (VisionComp && VisionConeMesh)
	{
		float Distance = VisionComp->VisionDistance;
		float FOV = VisionComp->FieldOfViewDegrees;

		float ScaleX = Distance / 100.0f;
		float ScaleYZ = ScaleX * FMath::Tan(FMath::DegreesToRadians(FOV));
		VisionConeMesh->SetRelativeScale3D(FVector(ScaleX, ScaleYZ, ScaleYZ));

		// CORRECCIÓN: Rotamos a +90 positivo (nariz hacia arriba). 
		// Esto invierte el cono: la punta se queda en el dron y la base se va lejos.
		VisionConeMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

		// Compensación para que la punta nazca exactamente del centro del dron
		VisionConeMesh->SetRelativeLocation(FVector(Distance, 0.0f, 0.0f));
	}

}
