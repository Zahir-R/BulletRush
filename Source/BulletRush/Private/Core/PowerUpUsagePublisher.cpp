#include "Core/PowerUpUsagePublisher.h"

void APowerUpUsagePublisher::MarkPowerUpUsed()
{
	NotifySubscribers();
}
