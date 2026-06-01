#include "Core/PuzzleEventPublisher.h"

void APuzzleEventPublisher::SetSolved(bool bInSolved)
{
	if (bInSolved && !bSolved)
	{
		bSolved = true;
		NotifySubscribers();
	}
}
