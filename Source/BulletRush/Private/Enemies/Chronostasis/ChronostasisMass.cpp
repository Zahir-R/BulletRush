#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Buffs/MassSlowDecorator.h"
#include "Components/SphereComponent.h"
#include "Components/BuffComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AChronostasisMass::AChronostasisMass()
{
    PrimaryActorTick.bCanEverTick = true;
    SlowZone = CreateDefaultSubobject<USphereComponent>(TEXT("SlowZone"));
    SlowZone->InitSphereRadius(400.f);
    SlowZone->SetupAttachment(RootComponent);
    AppliedDecorator = nullptr;
    MovementStrategy = MakeShareable(new FSeekMovement(200.f));
}

void AChronostasisMass::BeginPlay()
{
    Super::BeginPlay();
    if (SlowZone)
    {
        SlowZone->OnComponentBeginOverlap.AddDynamic(this, &AChronostasisMass::OnSlowZoneBeginOverlap);
        SlowZone->OnComponentEndOverlap.AddDynamic(this, &AChronostasisMass::OnSlowZoneEndOverlap);
    }
}

void AChronostasisMass::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (MovementStrategy.IsValid())
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaSeconds, PlayerPawn->GetActorLocation());
            SetActorLocation(NewPos);
        }
    }
}

// This is applied twice for some reason
void AChronostasisMass::OnSlowZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor == PlayerPawn && !bHasAppliedBuff)
    {
        UBuffComponent* BuffComp = OtherActor->FindComponentByClass<UBuffComponent>();
        if (BuffComp) {
            // Apply mass slow decorator via player's buff component
            AppliedDecorator = BuffComp->ApplyBuffAndReturn(UMassSlowDecorator::StaticClass(), -1.0f, 0.0f);

            if (AppliedDecorator) bHasAppliedBuff = true;
        }
    }
}

void AChronostasisMass::OnSlowZoneEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor == PlayerPawn && bHasAppliedBuff)
    {
        UBuffComponent* BuffComp = OtherActor->FindComponentByClass<UBuffComponent>();
        if (BuffComp && AppliedDecorator)
        {
            BuffComp->RemoveDecorator(AppliedDecorator);
            AppliedDecorator = nullptr;
            bHasAppliedBuff = false;
        }
    }
}

void AChronostasisMass::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Ensure decorator is removed
    if (AppliedDecorator)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
            if (BuffComp)
            {
                BuffComp->RemoveDecorator(AppliedDecorator);
            }
        }
        AppliedDecorator = nullptr;
    }
    Super::EndPlay(EndPlayReason);
}
