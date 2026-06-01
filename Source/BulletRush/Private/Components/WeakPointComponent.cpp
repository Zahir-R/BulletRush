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
	/* Definir características del punto debil
	MaxHealth = 50.0f;
    InitSphereRadius(40.0f); // Tamaño del punto débil
	*/
	// Detecta daño mediante Overlaps, siempre será true
    InitSphereRadius(40.0f);
	SetGenerateOverlapEvents(true);

    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crasheé sin querer----
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

	// Atamos la colisión a nosotros mismos
	OnComponentBeginOverlap.AddDynamic(this, &UWeakPointComponent::OnWeakPointOverlap);

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            PoolCache = GameInstance->GetSubsystem<UProjectilesSubsystem>();
            UE_LOG(LogTemp, Warning, TEXT("Pool cacheado correctamente"));
        }
    }
    //----Zona de Test, borralo o modificalo si no quieres que tu Unreal crasheé sin querer----
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

void UWeakPointComponent::OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
    // Ejemplo de como se haría daño
	
	if (OtherActor && OtherActor->ActorHasTag("BalaJugador"))
    {
        ABulletBase* Bullet = Cast<ABulletBase>(OtherActor);
        CurrentHealth -= Bullet->BulletData.Damage; // O el daño que haga la bala
        UE_LOG(LogTemp, Warning, TEXT("[%s] Recibió daño: %f, vida restante: %f"), *GetName(), Bullet->BulletData.Damage, CurrentHealth);
        PoolCache->ReturnBullet(Bullet);  // Retornamos la bala

        if (CurrentHealth <= 0.0f)
        {
            // ¡Punto débil destruido!
            SetGenerateOverlapEvents(false); // Ya no recibe más daño
            
            // Opcional: Reproducir explosión o sonido aquí
            UE_LOG(LogTemp, Warning, TEXT("[%s] fue destruido, ahora el jefe es vulnerable!!"), *GetName());

            VisualMesh->SetVisibility(false);

            // Tocamos el megáfono para avisarle al jefe
            OnDestroyedEvent.Broadcast(); 
        }
    }
}

void UWeakPointComponent::SetVisualMaterial(UMaterialInterface* NewMat)
{
    
    if (VisualMesh && NewMat)
        VisualMesh->SetMaterial(0, NewMat);
}

void UWeakPointComponent::ResetWeakPoint()
{
    CurrentHealth = MaxHealth;
    SetGenerateOverlapEvents(false); // Se reactivará en Open()
    if (VisualMesh) VisualMesh->SetVisibility(true);
}

void UWeakPointComponent::ForceDestroy()
{
    CurrentHealth = 0.0f;
    SetGenerateOverlapEvents(false);
    if (VisualMesh) VisualMesh->SetVisibility(false);
    OnDestroyedEvent.Broadcast();
}