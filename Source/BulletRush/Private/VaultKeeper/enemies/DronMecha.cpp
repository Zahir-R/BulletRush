// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultKeeper/enemies/DronMecha.h"
#include "VaultKeeper/objets/BatteryActor.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Combat/AttackPatterns.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ADronMecha::ADronMecha()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentDroneState = EDroneState::Closed;

    AttackInterval = 0.0f;    
    bAutoStartAttack = false; 
	// asignamos los materiales 
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpenMat(TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile_Pulse.M_Tech_Hex_Tile_Pulse'"));
    if (OpenMat.Succeeded())
        OpenMaterial = OpenMat.Object;
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ClosedMat(TEXT("Material'/Game/StarterContent/Materials/M_Tech_Hex_Tile.M_Tech_Hex_Tile'"));
    if (ClosedMat.Succeeded())
		ClosedMaterial = ClosedMat.Object;
}

void ADronMecha::BeginPlay()
{
    Super::BeginPlay();

    HomeLocation = GetActorLocation();
    // Usamos movimiento estático, sobreescribimos el sinusoidal de ADrone
    MovementStrategy = MakeShareable(new FStaticMovement());

    // Empezamos cerrado
    Close();
}

void ADronMecha::Tick(float DeltaSeconds)
{
    // Órbita horizontal alrededor del punto de origen
    OrbitAngle += OrbitSpeed * DeltaSeconds;

    float X = HomeLocation.X + FMath::Cos(OrbitAngle) * OrbitRadius;
    float Y = HomeLocation.Y + FMath::Sin(OrbitAngle) * OrbitRadius;
    float Z = HomeLocation.Z + FMath::Sin(GetWorld()->GetTimeSeconds() * HoverFrequency) * HoverAmplitude;

    SetActorLocation(FVector(X, Y, Z));

    // Siempre mira al jugador
    RotateTowardsPlayer(DeltaSeconds);
}

void ADronMecha::SetDroneState(EDroneState NewState)
{
    CurrentDroneState = NewState;
}

void ADronMecha::Open()
{
    SetDroneState(EDroneState::Open);
    HealthComp->SetInvulnerable(false);
    UE_LOG(LogTemp, Warning, TEXT("[DronMecha] Abierto — vulnerable"));

    if (MeshEnemy && OpenMaterial)
        MeshEnemy->SetMaterial(0, OpenMaterial);

    // Dispara guiado mientras está abierto
    StartAttack();

    // Después de OpenDuration se cierra
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer,
        this,
        &ADronMecha::Close,
        OpenDuration,
        false
    );
    GetWorld()->GetTimerManager().SetTimer(
        AttackLoopTimer,
        this,
        &ADronMecha::StartAttack,
        1.5f,
        true // loop mientras está abierto
    );
}

void ADronMecha::Close()
{
    SetDroneState(EDroneState::Closed);
    HealthComp->SetInvulnerable(true);
    UE_LOG(LogTemp, Warning, TEXT("[DronMecha] Cerrado — invulnerable"));

    if (MeshEnemy && ClosedMaterial)
        MeshEnemy->SetMaterial(0, ClosedMaterial);
    // Dispara anillo expansivo
    GetWorld()->GetTimerManager().ClearTimer(AttackLoopTimer);
    StartAttack();

    // Después de ClosedDuration se abre
    GetWorld()->GetTimerManager().SetTimer(
        CycleTimer,
        this,
        &ADronMecha::Open,
        ClosedDuration,
        false
    );
}

void ADronMecha::StartAttack()
{
    UBulletSpawnerComponent* Spawner = FindComponentByClass<UBulletSpawnerComponent>();
    if (!Spawner) return;

    if (CurrentDroneState == EDroneState::Closed)
    {
		// espiral expansiva
        TArray<FAttackStep> Pattern;
		Pattern.Add(FAttackStep(EAttackType::Spiral, 12, 300.0f, 0.1f, 20.0f, 15.0f)); // Un ataque de tipo espiral con 1 bala por paso, velocidad 300, delay de 0.1 segundos entre cada paso, y un parámetro especial de 20 grados de separación entre balas (aunque solo hay una bala por paso, este valor hará que cada bala nueva salga con una rotación adicional de 20 grados respecto a la anterior, creando el efecto de espiral)
        //Pattern.Add(FAttackStep(EAttackType::Circle, 12, 350.0f, 0.5f));
        Spawner->StartSequence(Pattern);
    }
    else
    {
        
        if (CurrentDroneState == EDroneState::Open)
        {
            APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
            if (!PlayerPawn) return;
            FVector Dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
   
		    if (Spawner)
            {
            float EffectiveSpeed = ProjectileSpeed * CurrentProjectileSpeedMultiplier;
            Spawner->InternalSpawn(GetActorLocation(), Dir, EffectiveSpeed, Damage);
            }  
            // EFECTO RETROCESO
            FVector BackwardDirection = -GetActorForwardVector();
            AddActorWorldOffset(BackwardDirection * 100.0f, true);
        }
    }
}

float ADronMecha::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentDroneState == EDroneState::Closed)
    {
        UE_LOG(LogTemp, Log, TEXT("[DronMecha] Cerrado — dano ignorado"));
        return 0.0f;
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void ADronMecha::RotateTowardsPlayer(float DeltaSeconds)
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!Player) return;

    FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = Direction.Rotation();
    FRotator CurrentRotation = GetActorRotation();

    SetActorRotation(FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed));
}


void ADronMecha::LinkBattery(ABatteryActor* Battery)
{
    LinkedBattery = Battery;
}

void ADronMecha::Update(APublisher* Publisher)
{
    LinkedBattery = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("[DronMecha] Bateria perdida — sin curacion"));
    // El drone sigue vivo pero ya no se cura
}

