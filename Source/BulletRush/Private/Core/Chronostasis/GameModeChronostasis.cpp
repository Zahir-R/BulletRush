#include "Core/Chronostasis/GameModeChronostasis.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Chronostasis/ChronostasisFacade.h"

void AGameModeChronostasis::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Iniciando GameMode"));
    Super::BeginPlay();
    // Find or spawn the facade actor
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("AGameModeChronostasis::BeginPlay: World es NULO"));
        return;
    }
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(World, AChronostasisFacade::StaticClass(), Found);
    AChronostasisFacade* Facade = nullptr;
    if (Found.Num() > 0)
    {
        Facade = Cast<AChronostasisFacade>(Found[0]);
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Se encontro un Facade existente en la escena: %s"), *Facade->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: No se encontro Facade en escena, procediendo a spawnear uno nuevo en (0,0,0)"));
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        Facade = World->SpawnActor<AChronostasisFacade>(AChronostasisFacade::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
    }
    if (Facade)
    {
        UE_LOG(LogTemp, Warning, TEXT("AGameModeChronostasis::BeginPlay: Llamando a Facade->StartGame()"));
        Facade->StartGame();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AGameModeChronostasis::BeginPlay: Error al obtener o spawnear el Facade"));
    }
}

void AGameModeChronostasis::ActivateSecretPortal()
{
    // Placeholder - actual portal activation implemented elsewhere
    UE_LOG(LogTemp, Log, TEXT("ActivateSecretPortal called"));
}

void AGameModeChronostasis::ActivateBossPortal()
{
    UE_LOG(LogTemp, Log, TEXT("ActivateBossPortal called"));
}
