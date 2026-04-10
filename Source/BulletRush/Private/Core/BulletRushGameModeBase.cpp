#include "../../Public/Core/BulletRushGameModeBase.h"
#include "../../Public/Player/PlayingPlayer.h"

ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
}