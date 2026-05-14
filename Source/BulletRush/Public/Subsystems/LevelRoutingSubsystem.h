// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelRoutingSubsystem.generated.h"

/**
 * 
 */
class UBulletRushGameInstance;

UCLASS()
class BULLETRUSH_API ULevelRoutingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// Función principal que llamara el Portal fisico ALevelPortal
	UFUNCTION()
	void SolicitarViajeANivel(FName NombreNivelDestino, const UObject* WorldContextObject);

	// Verifica si el jugador cumple los requisitos para entrar al nivel
	UFUNCTION()
	bool PuedeViajarANivel(FName NombreNivelDestino);

private:
	// Función interna para obtener tu GameInstance casteado de forma limpia
	UBulletRushGameInstance* GetBulletRushGameInstance() const;
};
