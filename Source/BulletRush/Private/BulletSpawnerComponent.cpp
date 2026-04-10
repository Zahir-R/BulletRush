#include "BulletSpawnerComponent.h"
#include "DrawDebugHelpers.h"

UBulletSpawnerComponent::UBulletSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBulletSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UBulletSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentSpiralAngle += 90.0f * DeltaTime;

	SpawnSpiral(1, 500.0f, CurrentSpiralAngle);
}

void UBulletSpawnerComponent::SpawnCircle(int32 BulletCount, float Speed)
{
	if (BulletCount <= 0) return;

	for (int32 i = 0; i < BulletCount; ++i)
	{
		float Angle = (PI * 2.0f / BulletCount) * i;
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);

		InternalSpawn(Direction, Speed);
	}
}

void UBulletSpawnerComponent::SpawnSpiral(int32 BulletCount, float Speed, float RotationOffset)
{
	for (int32 i = 0; i < BulletCount; ++i)
	{
		float Angle = (PI * 2.0f / BulletCount) * i + FMath::DegreesToRadians(RotationOffset);
		FVector Direction(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);

		InternalSpawn(Direction, Speed);
	}
}

void UBulletSpawnerComponent::SpawnBurst(int32 BulletCount, float Speed, float Interval)
{
	if (BulletCount <= 0) return;
	BulletsLeftToBurst = BulletCount;
	CachedBurstSpeed = Speed;
	
	GetWorld()->GetTimerManager().SetTimer(BurstTimerHandle, this, &UBulletSpawnerComponent::ExecuteBurstStep, Interval, true);
}

void UBulletSpawnerComponent::ExecuteBurstStep()
{
	if (BulletsLeftToBurst > 0)
	{
		FVector Forward = GetOwner()->GetActorForwardVector();
		InternalSpawn(Forward, CachedBurstSpeed);
		BulletsLeftToBurst--;
	}
	else GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
}

void UBulletSpawnerComponent::InternalSpawn(FVector Direction, float Speed)
{
	if (!GetOwner()) return;

	// TODO: Integrar con Object Pool
	UE_LOG(LogTemp, Warning, TEXT("Bala disparada hacia: %s a velocidad: %f"), *Direction.ToString(), Speed);

	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + Direction * 200.0f;

	DrawDebugDirectionalArrow(GetWorld(), Start, End, 50.0f, FColor::Red, false, 2.0f, 0, 2.0f);
}