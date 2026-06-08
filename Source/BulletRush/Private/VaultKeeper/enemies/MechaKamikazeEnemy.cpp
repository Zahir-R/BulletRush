#include "VaultKeeper/enemies/MechaKamikazeEnemy.h"
#include "Components/StaticMeshComponent.h"
//#include "Components/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AMechaKamikazeEnemy::AMechaKamikazeEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoStartAttack = false;
    AttackInterval = 0.0f;
    Tags.Add(FName("Enemigo"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/AccuCities/meshes/uasset/mesh/spaceship1.spaceship1'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
	MeshEnemy->SetRelativeScale3D(FVector(0.8f));
	//MeshEnemy->SetRelativeRotationExact(FRotator(0.0f, 0.0f, 90.0f));
	
}

void AMechaKamikazeEnemy::BeginPlay()
{
    Super::BeginPlay();
    bHasReachedTarget = false;

    if (MeshEnemy)
    {
        MeshEnemy->OnComponentBeginOverlap.AddDynamic(
            this, &AMechaKamikazeEnemy::OnOverlapPlayer);
    }
}

void AMechaKamikazeEnemy::Tick(float DeltaTime)
{
    if (bHasReachedTarget) return;
    Super::Tick(DeltaTime);

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation());
    float Dist = ToPlayer.Size();

    // Impacto por proximidad
    if (Dist < 100.0f)
    {
        bHasReachedTarget = true;
        UGameplayStatics::ApplyDamage(
            Player, ImpactDamage, GetController(), this, UDamageType::StaticClass());
        Die();
        return;
    }

    FVector Dir = ToPlayer.GetSafeNormal();
    FRotator Look = Dir.Rotation();
    Look.Pitch = 0.0f;
    Look.Roll = 0.0f;
    SetActorRotation(Look);
    SetActorLocation(GetActorLocation() + Dir * Speed * DeltaTime);
}

void AMechaKamikazeEnemy::OnOverlapPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasReachedTarget) return;
    if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;

    bHasReachedTarget = true;
    UGameplayStatics::ApplyDamage(
        OtherActor, ImpactDamage, GetController(), this, UDamageType::StaticClass());

    UE_LOG(LogTemp, Warning, TEXT("[MechaKamikaze] Impacto — daño: %.1f"), ImpactDamage);
    Die();
}

void AMechaKamikazeEnemy::Die()
{
    bHasReachedTarget = true;
	Super::Die();
}