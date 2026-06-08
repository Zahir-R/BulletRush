// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "PowerGenerator.generated.h"

class UEnemyHealthBarWidget;

UCLASS()
class BULLETRUSH_API APowerGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	APowerGenerator();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* HealthBarWidget;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnGeneratorDestroyed();

public:
	virtual FLinearColor GetHealthBarColor() const;
	virtual FLinearColor GetHealthBarColorLow() const;
	virtual FVector2D GetHealthBarSize() const { return FVector2D(120.f, 20.f); }
	virtual bool IsBoss() const { return false; }
	virtual FString GetBossDisplayName() const { return TEXT(""); }
	virtual bool ShouldShowBossName() const { return false; }
	virtual float GetHealthBarVerticalOffset() const { return 120.f; }

	UFUNCTION()
	void OnHealthUpdated(float NewHealth);
};
