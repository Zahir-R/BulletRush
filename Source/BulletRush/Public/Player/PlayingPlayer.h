#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BulletSpawnerComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BuffComponent.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatsInterface.h"
#include "PlayingPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UWeaponBaseComponent;
class UPlayerStatsBase;

UCLASS(Blueprintable)
class BULLETRUSH_API APlayingPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	APlayingPlayer();

	USpringArmComponent* CameraBoom;
	UCameraComponent* FollowCamera;
	UStaticMeshComponent* VisualMesh;

	// TESTING
	UBulletSpawnerComponent* Spawner;

protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	TScriptInterface<IPlayerStatsInterface> CurrentStats;

	UPROPERTY()
	UPlayerStatsBase* BaseStats;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void MoveUp(float Val);

	void OnFirePressed();
	void OnFireReleased();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void OnPlayerDeath();

	TArray<UWeaponBaseComponent*> EquippedWeapons;
	UPROPERTY()
	UWeaponBaseComponent* CurrentWeapon;
	UHealthComponent* HealthComp;
	UBuffComponent* BuffComp;

	UFUNCTION()
	void SelectWeapon1();

	UFUNCTION()
	void SelectWeapon2();

	UFUNCTION()
	void SelectWeapon3();

	UFUNCTION(BlueprintCallable)
	void SelectWeapon(int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WeaponWidgetClass;

	UPROPERTY()
	UUserWidget* WeaponWidget;

	void WrapStats(class UPlayerStatsDecorator* NewDecorator);
	void UnwrapStats();
	float GetTotalDamageMultiplier() const { return CurrentStats->GetDamageMultiplier(); }
	float GetTotalSpeedMultiplier() const { return CurrentStats->GetSpeedMultiplier(); }
	float GetTotalMaxHealthBonus() const { return CurrentStats->GetMaxHealthBonus(); }
	TScriptInterface<IPlayerStatsInterface> GetCurrentStats() const { return CurrentStats; }
	float GetTotalProjectileSpeedMultiplier() const { return CurrentStats->GetProjectileSpeedMultiplier(); }

	void UpdateMovementSpeed();

	void RefreshStatsFromChain();
};
