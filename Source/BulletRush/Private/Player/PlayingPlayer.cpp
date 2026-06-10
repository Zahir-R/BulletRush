#include "Player/PlayingPlayer.h"
#include "Player/PlayerStatsInterface.h"
#include "Player/PlayerStatsBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/HealthComponent.h"
#include "Components/WeaponBaseComponent.h"
#include "Components/ChargedWeaponComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
//////Strategies//////
#include "Components/Weapons/AutoFireStrategy.h"
#include "Components/Weapons/PlusFireStrategy.h"
#include "Components/Weapons/VolleyStrategy.h"
////////Buffs////////
#include "Buffs/PlayerStatsDecorator.h"
#include "Core/BulletRushGameInstance.h"
#include "Subsystems/MusicManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PauseMenuWidget.h"

APlayingPlayer::APlayingPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Player"));

	// MALLA
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/nave/Cube_011.Cube_011'"));
	if (MeshAsset.Succeeded())
	{
		VisualMesh->SetStaticMesh(MeshAsset.Object);
		VisualMesh->SetRelativeLocation(FVector(0, 0, 0)); // Donde se crear� la malla en el objeto
		VisualMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		VisualMesh->SetRelativeScale3D(
			FVector(
				10.f,  // Length
				10.f,  // Width
				10.f   // Thickness
			)
		);
	}

	// Hitbox (bullet hell — tiny hitbox, visual mesh and capsule ignore bullet traces)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	Hitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	Hitbox->SetupAttachment(RootComponent);
	Hitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	Hitbox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Hitbox->SetSphereRadius(15.0f);

	// C�mara
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 400.0f; // Distancia a la que se colocar� la c�mara
	CameraBoom->bUsePawnControlRotation = true; // C�mara rota con los controles
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 50.0f);

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

	GetCharacterMovement()->MaxAcceleration = 20000.0f;
	GetCharacterMovement()->BrakingDecelerationFlying = 20000.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = true;
	GetCharacterMovement()->GroundFriction = 0.0f;

	HealthComp= CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	HealthComp->MaxHealth = 100.0f;

	BuffComp = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComp"));

	static ConstructorHelpers::FClassFinder<UUserWidget>
		WidgetBPClass(
			TEXT("/Game/UI/WBP_WeaponSelection")
		);

	if (WidgetBPClass.Succeeded())
	{
		WeaponWidgetClass =
			WidgetBPClass.Class;
	}
	PauseMenuWidgetClass = LoadClass<UPauseMenuWidget>(nullptr, TEXT("/Script/BulletRush.PauseMenuWidget"));
}

void APlayingPlayer::BeginPlay()
{
	Super::BeginPlay();

	EquippedWeapons.Empty();

	UWeaponBaseComponent* AutoWeapon = NewObject<UWeaponBaseComponent>(this);
	UWeaponBaseComponent* VolleyWeapon = NewObject<UWeaponBaseComponent>(this);
	UWeaponBaseComponent* PlusWeapon = NewObject<UWeaponBaseComponent>(this);

	AutoWeapon->RegisterComponent();
	VolleyWeapon->RegisterComponent();
	PlusWeapon->RegisterComponent();

	AutoWeapon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	VolleyWeapon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	PlusWeapon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AutoWeapon->SetFireStrategy(NewObject<UAutoFireStrategy>(this));
	VolleyWeapon->SetFireStrategy(NewObject<UVolleyStrategy>(this));
	PlusWeapon->SetFireStrategy(NewObject<UPlusFireStrategy>(this));

	EquippedWeapons.Add(AutoWeapon);
	EquippedWeapons.Add(VolleyWeapon);
	EquippedWeapons.Add(PlusWeapon);

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	int32 WeaponIdx = 0;
	if (GI && EquippedWeapons.IsValidIndex(GI->CurrentWeaponIndex))
	{
		WeaponIdx = GI->CurrentWeaponIndex;
	}
	CurrentWeapon = EquippedWeapons[WeaponIdx];
	BaseStats = NewObject<UPlayerStatsBase>();
	CurrentStats = BaseStats;

	// TODO: Esto deberia hacer que el jugador muera, redirija al nivel CupHead y resetee el progreso
	if (HealthComp) HealthComp->OnDeath.AddDynamic(this, &APlayingPlayer::OnPlayerDeath);
	
	//<<<<<<< HEAD
	if (HealthComp)	HealthComp->CurrentHealth = HealthComp->MaxHealth;
	// Weapon 1 Strategy
	/*
	UVolleyStrategy* VolleyStrategy =
		NewObject<UVolleyStrategy>(this);

	TestWeapon->SetFireStrategy(VolleyStrategy);

	// Weapon 2 Strategy

	UPlusFireStrategy* PlusStrategy =
		NewObject<UPlusFireStrategy>(this);
	TestWeapontwo->SetFireStrategy(PlusStrategy);
=======
>>>>>>> origin/Final-Commit
*/}

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

	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &APlayingPlayer::SelectWeapon1);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &APlayingPlayer::SelectWeapon2);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &APlayingPlayer::SelectWeapon3);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayingPlayer::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayingPlayer::OnFireReleased);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &APlayingPlayer::TogglePauseMenu);
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
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void APlayingPlayer::OnFireReleased()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
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

	UE_LOG(LogTemp, Warning, TEXT("STATS: SPEED: %f, MAXHEALTH: %f, CURRENT HEALTH: %f"), NewSpeed, NewMaxHealth, HealthComp->CurrentHealth);
}

float APlayingPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (!HealthComp) return 0.0f;
    if (HealthComp->IsInvulnerable()) return 0.0f;

    float Dmg = HealthComp->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (HealthComp->CurrentHealth <= 0.0f) {
        return 0.0f;
    }

    HealthComp->SetInvulnerable(true, 1.0f);
    return Dmg;
}

void APlayingPlayer::OnPlayerDeath()
{
	Destroy();
}
void APlayingPlayer::SelectWeapon(int32 Index)
{
	if (!EquippedWeapons.IsValidIndex(Index))
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
	CurrentWeapon = EquippedWeapons[Index];

	UBulletRushGameInstance* GI = Cast<UBulletRushGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->CurrentWeaponIndex = Index;
	}
}
void APlayingPlayer::SelectWeapon1()
{
	SelectWeapon(0);
}

void APlayingPlayer::SelectWeapon2()
{
	SelectWeapon(1);
}

void APlayingPlayer::SelectWeapon3()
{
	SelectWeapon(2);
}

FVector APlayingPlayer::GetAimDirection() const
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return GetActorForwardVector();

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	return CamRot.Vector();
}

void APlayingPlayer::TogglePauseMenu()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
    {
        PauseMenuWidget->RemoveFromViewport();
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }
    else
    {
        if (!PauseMenuWidget && PauseMenuWidgetClass)
        {
            PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuWidgetClass);
            if (PauseMenuWidget)
            {
                PauseMenuWidget->OnVolverAlMapa.AddDynamic(this, &APlayingPlayer::OnPauseVolverAlMapa);
                PauseMenuWidget->OnContinuar.AddDynamic(this, &APlayingPlayer::OnPauseContinuar);
            }
        }
        if (PauseMenuWidget)
        {
            PauseMenuWidget->AddToViewport();
            PC->SetShowMouseCursor(true);
            PC->SetInputMode(FInputModeUIOnly());
            UGameplayStatics::SetGamePaused(GetWorld(), true);
        }
    }
}

void APlayingPlayer::OnPauseVolverAlMapa()
{
    if (PauseMenuWidget) PauseMenuWidget->RemoveFromViewport();
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
    }
    if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
        Music->NotifyLevelTravel();
    UGameplayStatics::OpenLevel(this, TEXT("Map_CupHeadMap"));
}

void APlayingPlayer::OnPauseContinuar()
{
    TogglePauseMenu();

}