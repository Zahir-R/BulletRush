// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Components/WeakPointComponent.h"
#include "../../Public/Combat/BulletBase.h"
#include "../../Public/Subsystems/ProjectilesSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/GameInstance.h"
#include "Materials/MaterialInstance.h"

UWeakPointComponent::UWeakPointComponent()
{
	/* Definir caracter�sticas del punto debil
	MaxHealth = 50.0f;
    InitSphereRadius(40.0f); // Tama�o del punto d�bil
	*/
	// Detecta da�o mediante Overlaps, siempre ser� true
    InitSphereRadius(40.0f);
	SetGenerateOverlapEvents(true);

    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crashe� sin querer----
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(this);



    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
    if (MeshAsset.Succeeded())
    {
        VisualMesh->SetStaticMesh(MeshAsset.Object);
    }

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("Material'/Game/StarterContent/Materials/M_CobbleStone_Smooth.M_CobbleStone_Smooth'"));

    if (MaterialAsset.Succeeded())
    {
        VisualMesh->SetMaterial(0, MaterialAsset.Object);
	}

    VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));

    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //-----------------------------------------------------------------------------------------
}

void UWeakPointComponent::BeginPlay()
{
	CurrentHealth = MaxHealth;

	// Atamos la colisi�n a nosotros mismos
	OnComponentBeginOverlap.AddDynamic(this, &UWeakPointComponent::OnWeakPointOverlap);

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            PoolCache = GameInstance->GetSubsystem<UProjectilesSubsystem>();
            UE_LOG(LogTemp, Warning, TEXT("Pool cacheado correctamente"));
        }
    }
    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crashe� sin querer----
    //Hecho con IA, no recomendado usarlo de todos modos xd
    if (VisualMesh && VisualMesh->GetStaticMesh())
    {
        float RadioDelModelo3D = VisualMesh->GetStaticMesh()->GetBounds().SphereRadius;

        if (RadioDelModelo3D > 0.0f)
        {
            float RadioDeLaColision = GetUnscaledSphereRadius();

            float EscalaPerfecta = RadioDeLaColision / RadioDelModelo3D;

            // 5. Aplicamos la escala en X, Y y Z
            VisualMesh->SetRelativeScale3D(FVector(EscalaPerfecta));
        }
    }
    //-----------------------------------------------------------------------------------------
}

void UWeakPointComponent::TakeDamageFromHit(float DamageAmount)
{
    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("[%s] Recibio danio: %f, vida restante: %f"), *GetName(), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.0f)
    {
        SetGenerateOverlapEvents(false);
        UE_LOG(LogTemp, Warning, TEXT("[%s] fue destruido, ahora el jefe es vulnerable!"), *GetName());
        VisualMesh->SetVisibility(false);
        OnDestroyedEvent.Broadcast();
    }
}

void UWeakPointComponent::OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("BalaJugador"))
    {
        ABulletBase* Bullet = Cast<ABulletBase>(OtherActor);
        TakeDamageFromHit(Bullet->BulletData.Damage);
        PoolCache->ReturnBullet(Bullet);
    }
}