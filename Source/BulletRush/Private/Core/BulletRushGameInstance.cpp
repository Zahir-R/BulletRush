// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BulletRushGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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

int32 UBulletRushGameInstance::GetVidasRestantes(FName NombreMapa)
{
	if (!VidasPorNivel.Contains(NombreMapa))
	{
		VidasPorNivel.Add(NombreMapa, 3);
	}
	return VidasPorNivel[NombreMapa];
}

int32 UBulletRushGameInstance::DecrementarVida(FName NombreMapa)
{
	int32 Vidas = GetVidasRestantes(NombreMapa);
	Vidas = FMath::Max(0, Vidas - 1);
	VidasPorNivel.Add(NombreMapa, Vidas);
	return Vidas;
}

void UBulletRushGameInstance::ResetVidas(FName NombreMapa)
{
	VidasPorNivel.Add(NombreMapa, 3);
}

void UBulletRushGameInstance::HandlePlayerDeath(UObject* WorldContextObject)
{
	if (!WorldContextObject) return;
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World, true);
	FName LevelFName(*CurrentLevelName);

	int32 Remaining = DecrementarVida(LevelFName);
	UE_LOG(LogTemp, Warning, TEXT("Player died. Vidas restantes para %s: %d"), *CurrentLevelName, Remaining);

	if (Remaining > 0)
	{
		// Reload the same level to reset player state
		UGameplayStatics::OpenLevel(WorldContextObject, LevelFName);
	}
	else
	{
		// No lives left: go back to CupHead map (map name 'CupHead')
		FName CupHeadMapName(TEXT("Map_CupHeadMap"));
		UGameplayStatics::OpenLevel(WorldContextObject, CupHeadMapName);
	}
}