#include "Player/PlayingPlayer.h"
#include "Player/PlayerStatsInterface.h"
#include "Player/PlayerStatsBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponBaseComponent.h"
#include "Components/ChargedWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Buffs/PlayerStatsDecorator.h"
#include "Buffs/DoubleDamage.h"
#include "Buffs/SpeedBoost.h"
#include "Buffs/HealthBonus.h"

APlayingPlayer::APlayingPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Player"));

	// MALLA
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Pipe.Shape_Pipe'"));
	if (MeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(MeshAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0, 0, -45)); // Donde se creará la malla en el objeto
	}

	// Cámara
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 400.0f; // Distancia a la que se colocará la cámara
	CameraBoom->bUsePawnControlRotation = true; // Cámara rota con los controles

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying; // Muy conveniente que exista esto xd
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->MaxFlySpeed = 1200.0f;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// No momentum
	float AbsurdAcceleration = 100000000.0f;
	GetCharacterMovement()->MaxAcceleration = AbsurdAcceleration;
	GetCharacterMovement()->BrakingDecelerationFlying = AbsurdAcceleration;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = false;

	TestWeapon = CreateDefaultSubobject<UWeaponBaseComponent>(TEXT("ArmaPrincipal"));
	TestWeapontwo = CreateDefaultSubobject<UChargedWeaponComponent>(TEXT("ArmaSecundaria"));
	TestWeapon->SetupAttachment(RootComponent);
	TestWeapontwo->SetupAttachment(RootComponent);
	TestWeapon->SetRelativeLocation(FVector(100.0f, 0.0f, 0.0f));
	TestWeapontwo->SetRelativeLocation(FVector(100.0f, 100.0f, 0.0f));

	HealthComp= CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->MaxHealth = 100.0f;

	BuffComp = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComp"));
}

void APlayingPlayer::BeginPlay()
{
	Super::BeginPlay();
	// TestWeapons
	
	EquippedWeapons.Add(TestWeapon);
	EquippedWeapons.Add(TestWeapontwo);

	BaseStats = NewObject<UPlayerStatsBase>();
	CurrentStats = BaseStats;
	
}

void APlayingPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayingPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayingPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayingPlayer::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &APlayingPlayer::MoveUp);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	/*
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &APlayingPlayer::TestCircle);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &APlayingPlayer::TestSpiral);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &APlayingPlayer::TestBurst);
	*/

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayingPlayer::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayingPlayer::OnFireReleased);
}

void APlayingPlayer::MoveForward(float Val)
{
	if (Controller && Val)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void APlayingPlayer::MoveRight(float Val)
{
	if (Controller && Val)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Val);
	}
}

void APlayingPlayer::MoveUp(float Val)
{
	if (Val) AddMovementInput(FVector::UpVector, Val);
}

void APlayingPlayer::OnFirePressed()
{
	for (UWeaponBaseComponent* Weapon : EquippedWeapons)
	{
		Weapon->StartFiring();
	}
}

void APlayingPlayer::OnFireReleased()
{
	for (UWeaponBaseComponent* Weapon : EquippedWeapons)
	{
		Weapon->StopFiring();
	}
}

void APlayingPlayer::WrapStats(UPlayerStatsDecorator* NewDecorator)
{
	if (!NewDecorator) return;
	NewDecorator->SetInner(CurrentStats);
	CurrentStats = NewDecorator;

	if (HealthComp)
	{
		float HealthRestore = CurrentStats->GetHealthRestore();
		if (HealthRestore > 0.0f) HealthComp->Heal(HealthRestore);
	}
	RefreshStatsFromChain();
}

void APlayingPlayer::UnwrapStats()
{
	UPlayerStatsDecorator* CurrentDec = Cast<UPlayerStatsDecorator>(CurrentStats.GetObject());
	if (!CurrentDec || !CurrentDec->GetInnerStats().GetObject()) return;

	CurrentStats = CurrentDec->GetInnerStats();
	RefreshStatsFromChain();
}

void APlayingPlayer::UpdateMovementSpeed()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		float BaseSpeed = 1200.0f;
		float NewSpeed = BaseSpeed * GetTotalSpeedMultiplier();
		MoveComp->MaxFlySpeed = NewSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Updated movement speed to %f"), NewSpeed);
	}
}


void APlayingPlayer::RefreshStatsFromChain()
{
	if (!HealthComp || !GetCharacterMovement()) return;

	float BaseSpeed = 1200.0f;
	float NewSpeed = BaseSpeed * GetTotalSpeedMultiplier();
	GetCharacterMovement()->MaxFlySpeed = NewSpeed;

	float NewMaxHealth = 100.0f + GetTotalMaxHealthBonus();
	HealthComp->MaxHealth = NewMaxHealth;
	if (HealthComp->CurrentHealth > NewMaxHealth) HealthComp->CurrentHealth = NewMaxHealth;

	UE_LOG(LogTemp, Warning, TEXT("STATS: SPEED: %f, MAXHEALTH: %f"), NewSpeed, NewMaxHealth);
}