#include "Core/Requirements/TimeStopRequirement.h"
#include "Core/Chronostasis/ChronostasisNormalFacade.h"
#include "Kismet/GameplayStatics.h"

void UTimeStopRequirement::Initialize(APlayerController* Player, UWorld* World)
{
	CurrentStops = 0;
	CachedPublisher = nullptr;
	if (!World) return;

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(World, AChronostasisNormalFacade::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		AChronostasisNormalFacade* Facade = Cast<AChronostasisNormalFacade>(Found[0]);
		if (Facade)
		{
			CachedPublisher = Facade;
			Facade->Subscribe(this);
		}
	}
}

void UTimeStopRequirement::Cleanup()
{
    if (CachedPublisher.IsValid())
    {
        CachedPublisher->Unsubscribe(this);
        CachedPublisher = nullptr;
    }
}

void UTimeStopRequirement::Update(APublisher* Publisher)
{
    CurrentStops++;
}
