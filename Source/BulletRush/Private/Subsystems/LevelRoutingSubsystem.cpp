// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Subsystems/LevelRoutingSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "../../Public/Core/BulletRushGameInstance.h"

UBulletRushGameInstance* ULevelRoutingSubsystem::GetBulletRushGameInstance() const
{
	// GetGameInstance viene heredado de UGameInstanceSubsystem
	return Cast<UBulletRushGameInstance>(GetGameInstance());
}

bool ULevelRoutingSubsystem::PuedeViajarANivel(FName NombreNivelDestino)
{
	UBulletRushGameInstance* GameInstance = GetBulletRushGameInstance();
	if (!GameInstance)
	{
		return false;
	}
	// true por defecto para testeo
	return true;
}

void ULevelRoutingSubsystem::SolicitarViajeANivel(FName NombreNivelDestino, const UObject* WorldContextObject)
{
	if (PuedeViajarANivel(NombreNivelDestino))
	{
		UE_LOG(LogTemp, Warning, TEXT("Navegando al mapa: %s"), *NombreNivelDestino.ToString());

		// guardamos estado antes de viajar si fuera necesario
		//el open level 
		UGameplayStatics::OpenLevel(WorldContextObject, NombreNivelDestino);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Acceso denegado al mapa: %s"), *NombreNivelDestino.ToString());
		// Dispara evento Portal Bloqueado mas adelante,time
	}
}