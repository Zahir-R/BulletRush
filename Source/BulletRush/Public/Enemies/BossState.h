#pragma once

#include "CoreMinimal.h"
#include "Enemies/BossBase.h"
#include "BossState.generated.h"

UCLASS(Abstract, BlueprintType)
class BULLETRUSH_API UBossState : public UObject
{
	GENERATED_BODY()

public:
	UBossState();

	virtual void EnterState(ABossBase* Boss);
	virtual void UpdateState(ABossBase* Boss, float DeltaTime);
	virtual void ExitState(ABossBase* Boss);

	UFUNCTION(BlueprintCallable, Category = "State")
	virtual FName GetStateTagName() const;
};

UCLASS()
class BULLETRUSH_API UBossStateIntro : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateIntro();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};

UCLASS()
class BULLETRUSH_API UBossStateIdle : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateIdle();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void UpdateState(ABossBase* Boss, float DeltaTime) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};

UCLASS()
class BULLETRUSH_API UBossStateAttacking : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateAttacking();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void UpdateState(ABossBase* Boss, float DeltaTime) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};

UCLASS()
class BULLETRUSH_API UBossStateStunned : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateStunned();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};

UCLASS()
class BULLETRUSH_API UBossStatePhaseTransition : public UBossState
{
	GENERATED_BODY()

public:
	UBossStatePhaseTransition();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};

UCLASS()
class BULLETRUSH_API UBossStateDead : public UBossState
{
	GENERATED_BODY()

public:
	UBossStateDead();
	virtual void EnterState(ABossBase* Boss) override;
	virtual void ExitState(ABossBase* Boss) override;
	virtual FName GetStateTagName() const override;
};
