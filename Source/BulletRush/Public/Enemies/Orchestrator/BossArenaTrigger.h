// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BossArenaTrigger.generated.h"

UCLASS()
class BULLETRUSH_API ABossArenaTrigger : public AActor
{
	GENERATED_BODY()

public:
	ABossArenaTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	// Donde aparecerá el jefe en la arena (Configurable en el editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Spawn", meta = (MakeEditWidget = true))
	FVector BossSpawnOffset;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};