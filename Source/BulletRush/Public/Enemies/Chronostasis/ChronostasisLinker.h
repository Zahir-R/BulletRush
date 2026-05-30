#pragma once
#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "ChronostasisLinker.generated.h"

class ABossBase;
class UPlayerStatsDecorator;

UCLASS(Blueprintable)
class BULLETRUSH_API AChronostasisLinker : public AEnemyBase
{
	GENERATED_BODY()
public:
	AChronostasisLinker();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// How close the player must be to the Linker-Boss line to be slowed
	UPROPERTY(EditAnywhere, Category = "Linker")
	float LinkLineWidth = 150.0f;

protected:
	TWeakObjectPtr<ABossBase> LinkedBoss;
	float SavedBossAttackInterval;

	UPROPERTY()
	UPlayerStatsDecorator* AppliedPlayerDebuff;

	UFUNCTION()
	void OnBossKilled(AEnemyBase* DeadBoss);

	bool IsPlayerOnLinkLine(const FVector& PlayerLocation) const;
};
