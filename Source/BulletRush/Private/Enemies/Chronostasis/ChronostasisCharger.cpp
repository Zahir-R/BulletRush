#include "Enemies/Chronostasis/ChronostasisCharger.h"
#include "Combat/MovementStrategy/SeekMovement.h"
#include "Components/BuffComponent.h"
#include "Buffs/ParalysisDecorator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

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
