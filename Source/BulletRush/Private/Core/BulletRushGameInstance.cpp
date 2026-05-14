// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BulletRushGameInstance.h"

bool UBulletRushGameInstance::IsMapaCompletado(FName NombreMapa)
{
	// Si el mapa existe en el diccionario y es true, devuelve true Si no, false
	return MapasCompletados.Contains(NombreMapa) && MapasCompletados[NombreMapa];
}

void UBulletRushGameInstance::MarcarMapaCompletado(FName NombreMapa)
{
	// Anade o actualiza el estado del mapa a completado
	MapasCompletados.Add(NombreMapa, true);
}