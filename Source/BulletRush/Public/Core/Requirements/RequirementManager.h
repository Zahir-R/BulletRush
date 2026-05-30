#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ILevelRequirement.h"
#include "RequirementManager.generated.h"

// Observer pattern: manager keeps track of requirements and can be queried/broadcast changes
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLETRUSH_API URequirementManager : public UActorComponent
{
    GENERATED_BODY()

public:
    URequirementManager();

    // Requirements configured in editor
    // Store UObject instances that implement the ILevelRequirement interface.
    // 'Instanced' is valid for UObject* arrays so the editor can create instances.
    UPROPERTY(EditAnywhere, Instanced, Category="Requirements")
    TArray<UObject*> SecretRequirements;

    // Simple multicast for UI or other systems to react when requirements change
    FSimpleMulticastDelegate OnRequirementsUpdated;

    // Initialize all registered requirements
    void InitializeRequirements(APlayerController* Player);

	// Check whether all secret requirements are met
	bool AreSecretRequirementsMet() const;

	// Whether any requirements have been configured
	bool HasRequirements() const { return SecretRequirements.Num() > 0; }

protected:
    virtual void BeginPlay() override;
};
