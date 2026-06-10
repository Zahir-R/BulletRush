#include "Player/TopDownPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATopDownPlayer::ATopDownPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    //MALLA VISUAL
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/nave/Cube_011.Cube_011'"));
    if (MeshAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(MeshAsset.Object);
        VisualMesh->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));
        VisualMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    // CÁMARA CINEMÁTICA CON LAG 
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);

    //  Rotar el brazo -90 grados en Pitch para que mire hacia abajo
    CameraBoom->SetRelativeRotation(FRotator(-70.0f, 0.0f, 0.0f));
    CameraBoom->TargetArmLength = 1500.0f; // Aumentado para que la cámara no esté pegada al piso

    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bDoCollisionTest = false;

    // Activando el suavizado Lag
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 5.0f; // 5.0 da un efecto más "cinemático" que 10.0

    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;

    // Cámara
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // RESTRICCIÓN DE EJES (PLANO 2D)
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        //Bloquear el eje Z por código
        MoveComp->bConstrainToPlane = true;
        MoveComp->SetPlaneConstraintNormal(FVector::UpVector); // Fija la restricción mirando hacia arriba (bloquea Z)
        MoveComp->SetPlaneConstraintOrigin(FVector::ZeroVector);

        //Ajustes base para que el jugador flote en el mapa
        MoveComp->SetMovementMode(MOVE_Flying);
        MoveComp->GravityScale = 0.0f;
        MoveComp->bOrientRotationToMovement = true;

        // No momentum
        float AbsurdAcceleration = 100000.0f;
        MoveComp->MaxAcceleration = AbsurdAcceleration;
        MoveComp->BrakingDecelerationFlying = AbsurdAcceleration;
        MoveComp->bUseSeparateBrakingFriction = true;
        MoveComp->BrakingFrictionFactor = 1.0f;
        MoveComp->BrakingFriction = 1000.0f;
        MoveComp->bRequestedMoveUseAcceleration = false;
        MoveComp->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
    }
}

void ATopDownPlayer::BeginPlay()
{
    Super::BeginPlay();
}

void ATopDownPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATopDownPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ATopDownPlayer::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATopDownPlayer::MoveRight);

}void ATopDownPlayer::MoveForward(float Val)
{
    AddMovementInput(FVector::ForwardVector, Val); // X axis
    
}

void ATopDownPlayer::MoveRight(float Val)
{
    AddMovementInput(FVector::RightVector, Val); // Y axis
    
}