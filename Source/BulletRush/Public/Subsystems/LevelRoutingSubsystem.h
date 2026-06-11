// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelRoutingSubsystem.generated.h"

/**
 * 
 */
class UBulletRushGameInstance;

UCLASS(Blueprintable)
class BULLETRUSH_API ULevelRoutingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// Funci�n principal que llamara el Portal fisico ALevelPortal
	UFUNCTION()
	void SolicitarViajeANivel(FName NombreNivelDestino, FName NivelRequerido, const UObject* WorldContextObject);

	// Verifica si el jugadro cumple los requisitos
	UFUNCTION()
	bool PuedeViajarANivel(FName NombreNivelDestino, FName NivelRequerido);

private:
	// Funci�n interna para obtener tu GameInstance casteado de forma limpia
	UBulletRushGameInstance* GetBulletRushGameInstance() const;
};
