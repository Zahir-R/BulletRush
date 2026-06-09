#include "Enemies/Chronostasis/ChronostasisExpansive.h"
#include "Enemies/Chronostasis/ChronostasisDrone.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/BuffComponent.h"
#include "Buffs/SlowDecorator.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AChronostasisExpansive::AChronostasisExpansive()
{
    PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Expansive/Temporal_Vortex_texture.Temporal_Vortex_texture'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Expansive/M_Temporal_Vortex.M_Temporal_Vortex'"));
	if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);
}

void AChronostasisExpansive::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &AChronostasisExpansive::TriggerPulse, 10.0f, true);
}

void AChronostasisExpansive::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(PulseTimerHandle);
    Super::EndPlay(EndPlayReason);
}

void AChronostasisExpansive::TriggerPulse()
{
    FVector MyLoc = GetActorLocation();
    float PulseRadius = 500.f;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChronostasisDrone::StaticClass(), Found);
    for (AActor* A : Found)
    {
        if (AChronostasisDrone* Drone = Cast<AChronostasisDrone>(A))
        {
            if (FVector::Dist(MyLoc, Drone->GetActorLocation()) <= PulseRadius)
            {
                Drone->ApplySpeedBuff(2.0f, 2.0f, 1.5f);
            }
        }
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn && FVector::Dist(MyLoc, PlayerPawn->GetActorLocation()) <= PulseRadius)
    {
        UBuffComponent* BuffComp = PlayerPawn->FindComponentByClass<UBuffComponent>();
        if (BuffComp)
        {
            BuffComp->ApplyBuff(USlowDecorator::StaticClass(), 2.0f, 0.0f);
        }
    }
}
