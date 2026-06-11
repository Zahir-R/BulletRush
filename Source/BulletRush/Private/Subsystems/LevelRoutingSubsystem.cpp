// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Subsystems/LevelRoutingSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "../../Public/Core/BulletRushGameInstance.h"
#include "Subsystems/MusicManagerSubsystem.h"

UBulletRushGameInstance* ULevelRoutingSubsystem::GetBulletRushGameInstance() const
{
	// GetGameInstance viene heredado de UGameInstanceSubsystem
	return Cast<UBulletRushGameInstance>(GetGameInstance());
}

bool ULevelRoutingSubsystem::PuedeViajarANivel(FName NombreNivelDestino, FName NivelRequerido)
{
	// Si no hay nivel requerido (Es el primer nivel), entra directo
	if (NivelRequerido.IsNone())
	{
		return true;
	}

	// Consultamos al GameInstance si el nivel requerido ya fue completado
	UBulletRushGameInstance* GameInstance = GetBulletRushGameInstance();
	if (GameInstance)
	{
		return GameInstance->IsMapaCompletado(NivelRequerido);
	}

	return false;
}

void ULevelRoutingSubsystem::SolicitarViajeANivel(FName NombreNivelDestino, FName NivelRequerido, const UObject* WorldContextObject)
{
	if (PuedeViajarANivel(NombreNivelDestino, NivelRequerido))
	{
		UE_LOG(LogTemp, Warning, TEXT("Navegando al mapa: %s"), *NombreNivelDestino.ToString());

		// guardamos estado antes de viajar si fuera necesario
		if (UMusicManagerSubsystem* Music = GetGameInstance()->GetSubsystem<UMusicManagerSubsystem>())
		{
			Music->NotifyLevelTravel();
		}
		//el open level 
		UGameplayStatics::OpenLevel(WorldContextObject, NombreNivelDestino);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Acceso denegado al mapa: %s"), *NombreNivelDestino.ToString());
		// Dispara evento Portal Bloqueado mas adelante,time
	}
}