#include "Core/Requirements/RequirementManager.h"
#include "Core/Requirements/RequirementManager.h"
#include "GameFramework/PlayerController.h"
#include "Core/Requirements/ILevelRequirement.h"
#include "Kismet/GameplayStatics.h"

URequirementManager::URequirementManager()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URequirementManager::BeginPlay()
{
    Super::BeginPlay();
}

void URequirementManager::InitializeRequirements(APlayerController* Player)
{
    UWorld* World = GetWorld();
    for (UObject* Obj : SecretRequirements)
    {
        if (!Obj) continue;
        if (Obj->GetClass()->ImplementsInterface(ULevelRequirement::StaticClass()))
        {
            ILevelRequirement* Req = Cast<ILevelRequirement>(Obj);
            if (Req)
            {
                Req->Initialize(Player, World);
            }
        }
    }
}

bool URequirementManager::AreSecretRequirementsMet() const
{
    if (SecretRequirements.Num() == 0) return false;
    for (UObject* Obj : SecretRequirements)
    {
        if (!Obj) continue;
        if (Obj->GetClass()->ImplementsInterface(ULevelRequirement::StaticClass()))
        {
            ILevelRequirement* Req = Cast<ILevelRequirement>(Obj);
            if (Req && !Req->IsCompleted()) return false;
        }
    }
    return true;
}
