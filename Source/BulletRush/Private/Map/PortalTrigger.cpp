#include "Map/PortalTrigger.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/PlayingPlayer.h"

APortalTrigger::APortalTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
    RootComponent = TriggerSphere;
    TriggerSphere->InitSphereRadius(200.f);
    TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}

void APortalTrigger::BeginPlay()
{
    Super::BeginPlay();
    if (TriggerSphere)
    {
        TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &APortalTrigger::OnOverlapBegin);
    }
}

void APortalTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsActive) return;
    if (!OtherActor) return;

    // Teleport only the player pawn
    if (APlayingPlayer* Player = Cast<APlayingPlayer>(OtherActor))
    {
        Player->SetActorLocation(TeleportTarget);
        OnPortalTriggered.Broadcast();
    }
}

void APortalTrigger::OnPortalOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Reserved for future use
}
