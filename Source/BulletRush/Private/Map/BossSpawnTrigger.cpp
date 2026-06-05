#include "Map/BossSpawnTrigger.h"
#include "Enemies/Bloodseeker/BloodseekerBoss.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABossSpawnTrigger::ABossSpawnTrigger()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;
    TriggerVolume->SetSphereRadius(200.0f);
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerVolume->SetCollisionResponseToAllChannels(ECR_Overlap);

    TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
    TriggerMesh->SetupAttachment(RootComponent);
    TriggerMesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

    PulseTime = 0.0f;
    ActivationDelay = 1.5f;
    bIsReady = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Tube.Shape_Tube'"));
    if (MeshFinder.Succeeded())
    {
        TriggerVisualMesh = MeshFinder.Object;
        TriggerMesh->SetStaticMesh(TriggerVisualMesh);
    }

    TriggerMesh->SetMaterial(0, nullptr);
}

void ABossSpawnTrigger::BeginPlay()
{
    Super::BeginPlay();
    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABossSpawnTrigger::OnOverlapBegin);
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FTimerHandle ActivationTimer;
    TWeakObjectPtr<ABossSpawnTrigger> WeakTrigger(this);
    GetWorld()->GetTimerManager().SetTimer(
        ActivationTimer,
        [WeakTrigger]()
        {
            ABossSpawnTrigger* Self = WeakTrigger.Get();
            if (!Self) return;
            Self->EnableTrigger();
        },
        ActivationDelay,
        false
    );
}

void ABossSpawnTrigger::EnableTrigger()
{
    bIsReady = true;
    TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossSpawnTrigger::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PulseTime += DeltaTime;
    float Pulse = FMath::Sin(PulseTime * 3.0f) * 0.15f;
    TriggerMesh->SetRelativeScale3D(FVector(2.0f + Pulse, 2.0f + Pulse, 2.0f + Pulse));
}

void ABossSpawnTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bIsReady || !OtherActor || !OtherActor->ActorHasTag("Player")) return;

    if (GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ABloodseekerBoss* NewBoss = GetWorld()->SpawnActor<ABloodseekerBoss>(
            BossClass ? BossClass : ABloodseekerBoss::StaticClass(),
            GetActorLocation() + FVector(0.0f, 0.0f, 100.0f),
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (NewBoss)
        {
            OnBossSpawned.Broadcast(NewBoss);
        }
    }

    Destroy();
}
