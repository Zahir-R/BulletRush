#include "Enemies/Chronostasis/ChronostasisMass.h"
#include "Buffs/SlowDecorator.h"
#include "Components/SphereComponent.h"
#include "Components/BuffComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

AChronostasisMass::AChronostasisMass()
{
    PrimaryActorTick.bCanEverTick = true;
    SlowZone = CreateDefaultSubobject<USphereComponent>(TEXT("SlowZone"));
    SlowZone->InitSphereRadius(400.f);
    SlowZone->SetupAttachment(RootComponent);
    AppliedDecorator = nullptr;
    MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
    USeekMovement* SeekMov = Cast<USeekMovement>(MovementStrategy);
    if (SeekMov) SeekMov->Speed = 200.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Mass/Aether_Cube_texture.Aether_Cube_texture'"));
    if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Mass/M_Aether_Cube.M_Aether_Cube'"));
    if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);

    MeshEnemy->SetWorldScale3D(FVector(0.5f));

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
    if (MovementStrategy)
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            const float StopDistance = 100.0f;
            const float StopDistanceSq = StopDistance * StopDistance;
            const float DistSq = FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation());

            if (DistSq <= StopDistanceSq) {
                FVector Curr = GetActorLocation();
                Curr = ApplyEnemySeparation(Curr);
                SetActorLocation(Curr);
            }
            else
            {
                FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaSeconds, PlayerPawn->GetActorLocation());
                NewPos = ApplyEnemySeparation(NewPos);
                SetActorLocation(NewPos);
            }
        }
    }
}

void AChronostasisMass::OnSlowZoneBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor == PlayerPawn && !bHasAppliedBuff)
    {
        UBuffComponent* BuffComp = OtherActor->FindComponentByClass<UBuffComponent>();
        if (BuffComp) {
            // Apply mass slow decorator via player's buff component
            AppliedDecorator = BuffComp->ApplyBuff(USlowDecorator::StaticClass(), -1.0f, 0.0f);

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
