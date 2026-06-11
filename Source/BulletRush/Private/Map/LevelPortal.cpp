// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/LevelPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Core/BulletRushGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TopDownPlayer.h"
#include "Subsystems/LevelRoutingSubsystem.h"

//mas testeo
#include "Player/PlayingPlayer.h"

// Sets default values
ALevelPortal::ALevelPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bIsExitPortal = true;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	CollisionBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	RootComponent = CollisionBox;


	//malla  para la caja
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	// descativamos las colisiones de la malla
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Assets/portal/Meshy_AI_Portal_of_the_Floatin_0607235308_texture.Meshy_AI_Portal_of_the_Floatin_0607235308_texture'"));

	if (MeshAsset.Succeeded())
	{
		PortalMesh->SetStaticMesh(MeshAsset.Object);
		//ubicacion respecto al box
		PortalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		PortalMesh->SetWorldScale3D(FVector(1.0f));
	}

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALevelPortal::OnOverlapBegin);
	//niagara
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(TEXT("NiagaraSystem'/Game/MixedVFX/Particles/Mix/NS_Mix_03.NS_Mix_03'"));
	if (NiagaraAsset.Succeeded())
	{
		NiagaraSystem = NiagaraAsset.Object;


		NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
		NiagaraComponent->SetAsset(NiagaraSystem);
		NiagaraComponent->SetupAttachment(RootComponent);
		NiagaraComponent->bAutoActivate = true;
	}


}

// Called when the game starts or when spawned
void ALevelPortal::BeginPlay()
{
	Super::BeginPlay();

}
void ALevelPortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
		NiagaraComponent->SetAsset(nullptr);
	}
	Super::EndPlay(EndPlayReason);
}

void ALevelPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Verificamos que el actor que chocó no sea nulo y no sea el portal mismo
	if (OtherActor && OtherActor != this)
	{
		// verificamos que el actor que choco sea el jugador 
		//referencia al player TopDown
		ATopDownPlayer* Player = Cast<ATopDownPlayer>(OtherActor);
		//testeo referencai al playingplayer
		APlayingPlayer* Player2 = Cast<APlayingPlayer>(OtherActor);
		if ((Player || Player2) && !TargetLevelName.IsNone())
		{
			if (!bIsUnlocked)
			{
				UE_LOG(LogTemp, Warning, TEXT("NIVEL BLOQUEADO. Debes pasar: %s"), *RequiredLevelToUnlock.ToString());
				return;
			}
			OnBeforeLevelTravel.Broadcast();
			EvaluarEstadoDeDesbloqueo();
			ULevelRoutingSubsystem* LevelRouter = GetGameInstance()->GetSubsystem<ULevelRoutingSubsystem>();
			if (LevelRouter)
			{
				if (bIsExitPortal)
				{
					LevelRouter->SolicitarViajeANivel(TargetLevelName, NAME_None, this);
				}
				else // Si es un portal del Hub, usamos la validación estricta
				{
					LevelRouter->SolicitarViajeANivel(TargetLevelName, RequiredLevelToUnlock, this);
				}
			}
			/*
			//cargamos el nivel corespondiente
			UGameplayStatics::OpenLevel(this, TargetLevelName);
			UE_LOG(LogTemp, Warning, TEXT("Playerrr a entrado al nivel %s"), *TargetLevelName.ToString());
			*/
		}
	}
}

// Called every frame
void ALevelPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelPortal::EvaluarEstadoDeDesbloqueo()
{

	if (bIsExitPortal)
	{
		bIsUnlocked = true;
	}
	else
	{
		bIsUnlocked = false;

		if (ULevelRoutingSubsystem* LevelRouter = GetGameInstance()->GetSubsystem<ULevelRoutingSubsystem>())
		{
			bIsUnlocked = LevelRouter->PuedeViajarANivel(TargetLevelName, RequiredLevelToUnlock);
		}
	}

	// Apagamos las partículas si está cerrado
	if (!bIsUnlocked && NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
	}
	else if (bIsUnlocked && NiagaraComponent)
	{
		NiagaraComponent->Activate();
	}
}