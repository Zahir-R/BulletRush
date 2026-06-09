#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Components/BuffComponent.h"
#include "Buffs/ParalysisDecorator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

AChronostasisCharger::AChronostasisCharger()
{
	PrimaryActorTick.bCanEverTick = true;

	MovementStrategy = CreateDefaultSubobject<USeekMovement>(TEXT("SeekMovement"));
	USeekMovement* Seek = Cast<USeekMovement>(MovementStrategy);
	if (Seek) Seek->Speed = 700.f;

	HitCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HitCollision"));
	HitCollision->InitSphereRadius(100.f);
	HitCollision->SetupAttachment(RootComponent);
	HitCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Charger/Nebula_Freight_texture.Nebula_Freight_texture'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
	MeshEnemy->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Charger/M_Nebula_Freight.M_Nebula_Freight'"));
	if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);
}

void AChronostasisCharger::BeginPlay()
{
	Super::BeginPlay();
	if (HitCollision)
	{
		HitCollision->OnComponentBeginOverlap.AddDynamic(this, &AChronostasisCharger::OnHitPlayer);
	}
}

void AChronostasisCharger::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (MovementStrategy)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			FVector NewPos = MovementStrategy->GetNextPosition(this, DeltaSeconds, PlayerPawn->GetActorLocation());
			NewPos = ApplyEnemySeparation(NewPos);
			SetActorLocation(NewPos);
		}
	}
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
		TargetRotation.Roll = 0.0f;

		FRotator CurrRotation = GetActorRotation();
		FRotator SmoothRotation = UKismetMathLibrary::RInterpTo(CurrRotation, TargetRotation, DeltaSeconds, 5.0f);
		SetActorRotation(SmoothRotation);
	}
}

void AChronostasisCharger::OnHitPlayer(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor == PlayerPawn)
	{
		UBuffComponent* Buff = PlayerPawn->FindComponentByClass<UBuffComponent>();
		if (Buff)
		{
			Buff->ApplyBuff(UParalysisDecorator::StaticClass(), 1.0f, 0.0f);
		}
		Die();
	}
}
