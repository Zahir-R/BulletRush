#include "Enemies/Chronostasis/ChronostasisLinker.h"
#include "Enemies/BossBase.h"
#include "Components/BuffComponent.h"
#include "Buffs/PlayerSlowDecorator.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"

AChronostasisLinker::AChronostasisLinker()
{
	PrimaryActorTick.bCanEverTick = true;
	AttackInterval = 0.f;
	bAutoStartAttack = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/ChronoEnemies/Linker/Astral_Prism_Pendant_texture.Astral_Prism_Pendant_texture'"));
	if (MeshAsset.Succeeded()) MeshEnemy->SetStaticMesh(MeshAsset.Object);
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/ChronoEnemies/Linker/M_Astral_Prism_Pendant.M_Astral_Prism_Pendant'"));
	if (MaterialAsset.Succeeded()) MeshEnemy->SetMaterial(0, MaterialAsset.Object);
}

void AChronostasisLinker::BeginPlay()
{
	Super::BeginPlay();

	AActor* Found = UGameplayStatics::GetActorOfClass(GetWorld(), ABossBase::StaticClass());
	LinkedBoss = Cast<ABossBase>(Found);
	if (LinkedBoss.IsValid())
	{
		LinkedBoss->OnEnemyDeath.AddDynamic(this, &AChronostasisLinker::OnBossKilled);
		SavedBossAttackInterval = LinkedBoss->AttackInterval;
		LinkedBoss->AttackInterval = SavedBossAttackInterval / 1.5f;
	}
}

bool AChronostasisLinker::IsPlayerOnLinkLine(const FVector& PlayerLocation) const
{
	if (!LinkedBoss.IsValid()) return false;

	FVector LineStart = GetActorLocation();
	FVector LineEnd = LinkedBoss->GetActorLocation();
	FVector LineDir = LineEnd - LineStart;
	float LineLength = LineDir.Size();
	if (LineLength <= KINDA_SMALL_NUMBER) return false;
	LineDir /= LineLength;

	// Project player position onto the line
	FVector StartToPlayer = PlayerLocation - LineStart;
	float Projection = FVector::DotProduct(StartToPlayer, LineDir);

	// Player must be between linker and boss (0 <= t <= 1)
	if (Projection < 0.0f || Projection > LineLength) return false;

	// Distance from the line
	FVector PointOnLine = LineStart + LineDir * Projection;
	float DistFromLine = FVector::Dist(PlayerLocation, PointOnLine);

	return DistFromLine <= LinkLineWidth;
}

void AChronostasisLinker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!LinkedBoss.IsValid())
	{
		Die();
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		if (AppliedPlayerDebuff)
		{
			AppliedPlayerDebuff = nullptr;
		}
		return;
	}

	bool bOnLine = IsPlayerOnLinkLine(PlayerPawn->GetActorLocation());

	if (bOnLine && !AppliedPlayerDebuff)
	{
		UBuffComponent* Buff = PlayerPawn->FindComponentByClass<UBuffComponent>();
		if (Buff)
		{
			AppliedPlayerDebuff = Buff->ApplyBuff(UPlayerSlowDecorator::StaticClass(), -1.0f, 0.7f);
		}
	}
	else if (!bOnLine && AppliedPlayerDebuff)
	{
		UBuffComponent* Buff = PlayerPawn->FindComponentByClass<UBuffComponent>();
		if (Buff)
		{
			Buff->RemoveDecorator(AppliedPlayerDebuff);
		}
		AppliedPlayerDebuff = nullptr;
	}

	DrawDebugLine(GetWorld(), GetActorLocation(), LinkedBoss->GetActorLocation(), FColor::Purple, false, -1.f, 0, 4.f);
}

void AChronostasisLinker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LinkedBoss.IsValid())
	{
		LinkedBoss->AttackInterval = SavedBossAttackInterval;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn && AppliedPlayerDebuff)
	{
		UBuffComponent* Buff = PlayerPawn->FindComponentByClass<UBuffComponent>();
		if (Buff)
		{
			Buff->RemoveDecorator(AppliedPlayerDebuff);
		}
		AppliedPlayerDebuff = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AChronostasisLinker::OnBossKilled(AEnemyBase* DeadBoss)
{
	if (DeadBoss == LinkedBoss.Get())
	{
		LinkedBoss = nullptr;
		Die();
	}
}
