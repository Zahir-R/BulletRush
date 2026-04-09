#include "BulletRushGameModeBase.h"
#include "PlayingPlayer.h"

ABulletRushGameModeBase::ABulletRushGameModeBase()
{
	DefaultPawnClass = APlayingPlayer::StaticClass();
}