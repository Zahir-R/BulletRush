// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BulletRushGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BULLETRUSH_API UBulletRushGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	// la variable nos sirve para ("Nivel_Boss1" -> true/false ) si se completo o no
	UPROPERTY()
	TMap<FName, bool> MapasCompletados;

	// Funcinn rapida para consultar si un mapa ya se pasó
	UFUNCTION()
	bool IsMapaCompletado(FName NombreMapa);

	// Funcion para registrar jefe completado
	UFUNCTION()
	void MarcarMapaCompletado(FName NombreMapa);
};
