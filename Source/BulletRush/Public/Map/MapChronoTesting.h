// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapChronoTesting.generated.h"

UCLASS()
class BULLETRUSH_API AMapChronoTesting : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapChronoTesting();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Map", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MapMeshComponent;

};
