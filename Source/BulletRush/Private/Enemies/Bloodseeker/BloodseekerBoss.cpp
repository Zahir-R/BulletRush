

#include "Enemies/Bloodseeker/BloodseekerBoss.h"

#include "Player/PlayingPlayer.h" 
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABloodseekerBoss::ABloodseekerBoss()
{
    
    PrimaryActorTick.bCanEverTick = true;

    // Inicialización de variables de control de movimiento cinemático
    MovementAmplitude = 350.0f; 
    MovementFrequency = 1.5f;  

    // Inicialización de valores para el ataque especial Rupture
    bIsRuptureActive = false;
    RuptureDamageMultiplier = 0.15f; 
    RuptureDuration = 6.0f;          
    LastPlayerLocation = FVector::ZeroVector;

    TargetPlayer = nullptr;
}

void ABloodseekerBoss::BeginPlay()
{
    Super::BeginPlay();

    
    InitialLocation = GetActorLocation();

    // Polimorfismo 
    AActor* FoundPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (FoundPlayer)
    {
        TargetPlayer = Cast<APlayingPlayer>(FoundPlayer);
    }

   
    CustomBurstCombo.Add(FAttackStep(EAttackType::Burst, 8, 700.0f, 0.1f, 0.05f));
}

void ABloodseekerBoss::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetPlayer || GetCurrentBossStateName() == "Dead") return;

    //ROTACIÓN EN SU PROPIO EJE 
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPlayer->GetActorLocation());
   
    LookAtRotation.Pitch = 0.0f;
    LookAtRotation.Roll = 0.0f;
    SetActorRotation(LookAtRotation);

   
    
    if (GetCurrentBossStateName() != "Stunned")
    {
        FVector NewLocation = InitialLocation;
        float TimeSecs = GetWorld()->GetTimeSeconds();
     
        NewLocation += GetActorRightVector() * FMath::Sin(TimeSecs * MovementFrequency) * MovementAmplitude;
        SetActorLocation(NewLocation);
    }

  
    if (bIsRuptureActive)
    {
        FVector CurrentPlayerLocation = TargetPlayer->GetActorLocation();

        // Calculamos la distancia euclidiana
        float DistanceMoved = FVector::Distance(CurrentPlayerLocation, LastPlayerLocation);

        if (DistanceMoved > 0.1f)
        {
            float CalculatedDamage = DistanceMoved * RuptureDamageMultiplier;

            
            UGameplayStatics::ApplyDamage(TargetPlayer, CalculatedDamage, GetController(), this, UDamageType::StaticClass());

            UE_LOG(LogTemp, Warning, TEXT("[BLOODSEEKER] El jugador se movió %f cm. Daño aplicado: %f"), DistanceMoved, CalculatedDamage);
        }

        
        LastPlayerLocation = CurrentPlayerLocation;
    }
}

void ABloodseekerBoss::Attack()
{
    if (!BulletSpawner || !TargetPlayer) return;

  
    switch (AttackIdentifier)
    {
    case 0:
        
        BulletSpawner->StartSequence(CustomBurstCombo);
        break;

    case 1:
        //Invocación del ataque circular masivo
        BulletSpawner->StartSequence(Combo);
        break;

    case 2:
        //Activación del Ataque Especial Rupture
        ExecuteRuptureAttack();
        break;

    default:
        // Por Parte de la herencia
        Super::Attack();
        break;
    }
}

void ABloodseekerBoss::ExecuteRuptureAttack()
{
    if (bIsRuptureActive || !TargetPlayer) return;

    bIsRuptureActive = true;
    LastPlayerLocation = TargetPlayer->GetActorLocation();

    UE_LOG(LogTemp, Error, TEXT("[!!ESPECIAL!!] Bloodseeker activó RUPTURE en el jugador. ¡Moverse causará daño mortal!"));

    BulletSpawner->StartSequence(Combo2);

    
    GetWorld()->GetTimerManager().SetTimer(
        RuptureTimerHandle,
        this,
        &ABloodseekerBoss::DeactivateRupture,
        RuptureDuration,
        false
    );
}

void ABloodseekerBoss::DeactivateRupture()
{
    bIsRuptureActive = false;
    UE_LOG(LogTemp, Log, TEXT("[BLOODSEEKER] Estado Rupture finalizado. El jugador puede moverse libremente."));

    
    GetWorld()->GetTimerManager().ClearTimer(RuptureTimerHandle);
}

