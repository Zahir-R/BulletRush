// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Components/WeakPointComponent.h"

UWeakPointComponent::UWeakPointComponent()
{
	/* Definir características del punto debil
	MaxHealth = 50.0f;
    InitSphereRadius(40.0f); // Tamaño del punto débil
	*/
	// Detecta daño mediante Overlaps, siempre será true
    InitSphereRadius(30.0f);
	SetGenerateOverlapEvents(true);
}

void UWeakPointComponent::BeginPlay()
{
	CurrentHealth = MaxHealth;

	// Atamos la colisión a nosotros mismos
	OnComponentBeginOverlap.AddDynamic(this, &UWeakPointComponent::OnWeakPointOverlap);
}

void UWeakPointComponent::OnWeakPointOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
    // Ejemplo de como se haría daño
	
	if (OtherActor && OtherActor->ActorHasTag("BalaJugador"))
    {
        CurrentHealth -= OtherActor->BulletData.Damage; // O el daño que haga la bala
        // OtherActor->Destroy();  // Destruimos la bala

        if (CurrentHealth <= 0.0f)
        {
            // ¡Punto débil destruido!
            SetGenerateOverlapEvents(false); // Ya no recibe más daño
            
            // Opcional: Reproducir explosión o sonido aquí

            // Tocamos el megáfono para avisarle al jefe
            OnDestroyedEvent.Broadcast(); 
        }
    }
}