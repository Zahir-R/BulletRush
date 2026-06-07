// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Orchestrator/SurveillanceDrone.h"
#include "Components/StealthVisionComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "Enemies/Orchestrator/SurveillanceAIControler.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SpotLightComponent.h"

ASurveillanceDrone::ASurveillanceDrone()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(50.0f);
	CollisionComp->SetMobility(EComponentMobility::Movable);

	CollisionComp->SetCollisionProfileName(TEXT("Pawn"));
	CollisionComp->SetCanEverAffectNavigation(false);

	SetRootComponent(CollisionComp);
	
	// 2. ADJUNTAMOS LA MALLA A LA ESFERA
	MeshEnemy->SetupAttachment(CollisionComp); // <-- Asegúrate de adjuntarlo al CollisionComp
	MeshEnemy->SetMobility(EComponentMobility::Movable);
	MeshEnemy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshEnemy->SetSimulatePhysics(false);
	MeshEnemy->SetCanEverAffectNavigation(false);
	MeshEnemy->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f)); // Centrado en la esfera

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

	SpotLightComp = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComp"));
	SpotLightComp->SetupAttachment(MeshEnemy);
	SpotLightComp->SetLightColor(FLinearColor::Red); // Color de la luz (Rojo alerta)
	SpotLightComp->SetIntensity(5000.0f);


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

		// 1. ESCALA Z: La profundidad/altura real del Shape_Cone de Unreal
		float ScaleZ = Distance / 100.0f;

		// 2. ESCALA X e Y: El radio real calculado trigonométricamente
		float Radius = Distance * FMath::Tan(FMath::DegreesToRadians(FOV));
		// Dividimos entre 50.0f porque el radio por defecto del Shape_Cone es 50
		float ScaleXY = Radius / 50.0f;

		// Aplicamos la escala respetando los ejes del modelo nativo
		VisionConeMesh->SetRelativeScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));

		// 3. ROTACIÓN: Acostamos el cono de forma que la punta mire hacia el dron (Local -X)
		VisionConeMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

		// 4. POSICIÓN: Como el pivote está en el centro geométrico del cono,
		// solo debemos empujarlo LA MITAD de la distancia para que la punta se ancle en el dron.
		VisionConeMesh->SetRelativeLocation(FVector(Distance, 0.0f, 0.0f));

		if (SpotLightComp)
		{
			// El radio de la luz será igual a la distancia de visión
			SpotLightComp->SetAttenuationRadius(Distance);
			// El ángulo exterior del foco será igual a tu ángulo de visión
			SpotLightComp->SetOuterConeAngle(FOV);
			// El ángulo interior un poco menor para un degradado suave
			SpotLightComp->SetInnerConeAngle(FOV * 0.8f);
		}
	}

}
