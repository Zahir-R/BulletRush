#include "../../Public/Components/BulletSpawnerComponent.h"
#include "../../Public/Combat/AttackPatterns.h"
#include "DrawDebugHelpers.h"

UBulletSpawnerComponent::UBulletSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UBulletSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AttackRegist.Add(EAttackType::Circle, MakeShared<FCircleAttack>());
	AttackRegist.Add(EAttackType::Spiral, MakeShared<FSpiralAttack>());
	AttackRegist.Add(EAttackType::Burst, MakeShared<FBurstAttack>());
	// Otros patrones

}


void UBulletSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UBulletSpawnerComponent::InternalSpawn(FVector Origin, FVector Direction, float Speed)
{
	if (!GetOwner()) return;

	// TODO: Integrar con Object Pool
	UE_LOG(LogTemp, Warning, TEXT("Bala disparada hacia: %s a velocidad: %f"), *Direction.ToString(), Speed);

	FVector End = Origin + Direction * 200.0f;

	DrawDebugDirectionalArrow(GetWorld(), Origin, End, 50.0f, FColor::Red, false, 2.0f, 0, 2.0f);
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
		FAttackParams Params{ Step.BulletCount, Step.Speed, Step.DelayAfter, Step.SpecialParam, SpawnOrigin };
		AttackRegist[Step.Type]->Execute(this, Params);
	}

	CurrentStepIndex++;

	if (CurrentSequence.IsValidIndex(CurrentStepIndex))
	{
		GetWorld()->GetTimerManager().SetTimer(SequenceTimerHandle, this, &UBulletSpawnerComponent::ExecuteNextStep, Step.DelayAfter, false);
	}
}