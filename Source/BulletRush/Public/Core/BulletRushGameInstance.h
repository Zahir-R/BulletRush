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

	// Vidas por nivel (inicialmente 3 por nivel)
	UPROPERTY()
	TMap<FName, int32> VidasPorNivel;

	// Obtiene las vidas restantes para un nivel (inicializa a 3 si no existe)
	UFUNCTION()
	int32 GetVidasRestantes(FName NombreMapa);

	// Decrementa una vida para el nivel y devuelve las vidas restantes
	UFUNCTION()
	int32 DecrementarVida(FName NombreMapa);

	// Maneja la muerte del jugador: si quedan vidas reinicia el nivel; si no, vuelve al mapa CupHead
    UFUNCTION()
	void HandlePlayerDeath(UObject* WorldContextObject);

	// Funcinn rapida para consultar si un mapa ya se pasó
	UFUNCTION()
	bool IsMapaCompletado(FName NombreMapa);

	// Funcion para registrar jefe completado
	UFUNCTION()
	void MarcarMapaCompletado(FName NombreMapa);
};
