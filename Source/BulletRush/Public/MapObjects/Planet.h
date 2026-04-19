// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Planet.generated.h"

//class USphereComponent;

UCLASS()
class BULLETRUSH_API APlanet : public AActor
{
	GENERATED_BODY()
	
public:
	APlanet();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
    /*
    UPROPERTY(VisibleAnywhere)
    USphereComponent* RangeSphere;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText PlanetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LevelToLoad;

    UFUNCTION()
    void OnEnterRange(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    UFUNCTION()
    void OnExitRange(UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex);*/
	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

	UPROPERTY(EditAnywhere)
	float UseRange = 300.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
