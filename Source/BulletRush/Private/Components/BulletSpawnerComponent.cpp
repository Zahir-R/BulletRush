#include "Components/BulletSpawnerComponent.h"
#include "Combat/AttackPatterns/AttackStrategy.h"
#include "Combat/AttackPatterns/BurstAttack.h"
#include "Combat/AttackPatterns/CircleAttack.h"
#include "Combat/AttackPatterns/SpiralAttack.h"
#include "Combat/AttackPatterns/SphereAttack.h"

UBulletSpawnerComponent::UBulletSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UBulletSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		ProjectilesSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UProjectilesSubsystem>();
	}
	
    AttackRegist.Add(EAttackType::Circle, TStrongObjectPtr<UAttackStrategy>(NewObject<UCircleAttack>(this)));
	AttackRegist.Add(EAttackType::Sphere, TStrongObjectPtr<UAttackStrategy>(NewObject<USphereAttack>(this)));
	AttackRegist.Add(EAttackType::Spiral, TStrongObjectPtr<UAttackStrategy>(NewObject<USpiralAttack>(this)));
	AttackRegist.Add(EAttackType::Burst, TStrongObjectPtr<UAttackStrategy>(NewObject<UBurstAttack>(this)));
	// Otros patrones

}


void UBulletSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UBulletSpawnerComponent::InternalSpawn(FVector Origin, FVector Direction, float Speed, float Damage, FVector Scale)
{
	if (!GetOwner() || !ProjectilesSubsystem) return;

	FVector SpawnLocation = Origin + Direction * 150.0f;
	AActor* OwnerActor = GetOwner();

	ABulletBase* Bullet = ProjectilesSubsystem->RequestBullet(
        SpawnLocation, Direction, Speed, bIsPlayerSource, Damage, SpawnLocation, OwnerActor, Scale
	);

	if (Bullet)
	{
		//UE_LOG(LogTemp, Log, TEXT("Bala disparada desde %s hacia %s"), *Origin.ToString(), *Direction.ToString());
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("No se pudo generar la bala o no hay disponibles en el pool"));
	}
}

void UBulletSpawnerComponent::StartSequence(const TArray<FAttackStep>& NewSequence)
{
	if (NewSequence.Num() == 0) return;
	CurrentSequence = NewSequence;
	CurrentStepIndex = 0;
	ExecuteNextStep();
}

void UBulletSpawnerComponent::ExecuteNextStep()
{
	if (CurrentStepIndex >= CurrentSequence.Num()) return;

	FAttackStep& Step = CurrentSequence[CurrentStepIndex];

	FVector SpawnOrigin = Step.bUseBossLocation && GetOwner() ?
		GetOwner()->GetActorLocation() :
		Step.CustomOrigin;

	if (AttackRegist.Contains(Step.Type))
	{
		FAttackParams Params{ 
			Step.BulletCount,
			Step.Speed,
			Step.DelayAfter,
			Step.SpecialParam,
			SpawnOrigin,
			Step.Damage,
			Step.BulletScale
		};
		AttackRegist[Step.Type]->Execute(this, Params);
	}

	CurrentStepIndex++;

	if (CurrentSequence.IsValidIndex(CurrentStepIndex))
	{
		GetWorld()->GetTimerManager().SetTimer(SequenceTimerHandle, this, &UBulletSpawnerComponent::ExecuteNextStep, Step.DelayAfter, false);
	}
}

void UBulletSpawnerComponent::StopCurrentSequence()
{
	if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(SequenceTimerHandle);
	CurrentStepIndex = 0;
	CurrentSequence.Empty();
}