#include "Core/CollectiblePickupPublisher.h"

void ACollectiblePickupPublisher::RegisterPick()
{
	NotifySubscribers();
}
