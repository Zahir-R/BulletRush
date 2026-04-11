#include "../../Public/Core/BulletRushGameModeBase.h"
#include "../../Public/Player/PlayingPlayer.h"
#include "../../Public/Player/TopDownPlayer.h"
#include "Engine/World.h"

ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
	
}
UClass* ABulletRushGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (GetWorld())
	{
		FString LevelName = GetWorld()->GetMapName();
		// NOTA: Usamos "Contains" porque al darle Play en el editor, Unreal a veces 
		// le pone prefijos raros al nombre del mapa (ej. "UEDPIE_0_Map_CupHeadMap")
		if (LevelName.Contains("Map_CupHeadMap"))
		{
			// Si estamos en tu mapa, usamos nave TopDown
			return ATopDownPlayer::StaticClass();
		}
	}
	// Si estamos en CUALQUIER otro nivel  usa  playingplayer por el momento 
	return APlayingPlayer::StaticClass();
}