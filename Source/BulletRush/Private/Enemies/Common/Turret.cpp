#include "Enemies/Common/Turret.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackInterval = 0.5f;

	// Turret doesn't move
	bAutoStartAttack = false;
}
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	BeginAttackLoop();
}
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* Player =
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!Player)
	{
		return;
	}

	FVector Direction =
		Player->GetActorLocation() -
		GetActorLocation();

	SetActorRotation(
		FRotator(
			0.f,
			Direction.Rotation().Yaw,
			0.f
		)
	);
}
void ATurret::StartAttack()
{
	if (!BulletSpawner)
	{
		return;
	}
		
	APawn* PlayerPawn =
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn)
	{	
		return;
	}

	FVector Dir =
		(PlayerPawn->GetActorLocation() -
			GetActorLocation()).GetSafeNormal();

	BulletSpawner->InternalSpawn(
		GetActorLocation(),
		Dir,
		1000.f,
		20.f
	);
}