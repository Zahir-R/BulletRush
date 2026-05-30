#include "Core/Requirements/TimeStopRequirement.h"
#include "Core/Chronostasis/ChronostasisFacade.h"
#include "Kismet/GameplayStatics.h"

void UTimeStopRequirement::Initialize(APlayerController* Player, UWorld* World)
{
    CurrentStops = 0;
    if (!World) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AChronostasisFacade::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        AChronostasisFacade* Facade = Cast<AChronostasisFacade>(Found[0]);
        if (Facade)
        {
            ObservedFacade = Facade;
            Facade->OnTimeStop.AddUObject(this, &UTimeStopRequirement::RegisterTimeStop);
        }
    }
}

void UTimeStopRequirement::Cleanup()
{
    if (ObservedFacade.IsValid())
    {
        ObservedFacade->OnTimeStop.RemoveAll(this);
    }
}

void UTimeStopRequirement::RegisterTimeStop()
{
    CurrentStops++;
}
