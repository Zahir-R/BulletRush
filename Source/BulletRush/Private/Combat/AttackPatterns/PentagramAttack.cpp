// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/AttackPatterns/PentagramAttack.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UPentagramAttack::Execute(UBulletSpawnerComponent* Spawner, const FAttackParams& Params)
{
	if (!Spawner || !Spawner->GetOwner()) return;

	// Obtenemos al jugador actual
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Spawner->GetWorld(), 0);
	if (!PlayerPawn) return;

	float StaffWidth = Params.SpecialParam > 0.0f ? Params.SpecialParam : 7000.0f;
	float LineSpacing = 380.0f;
	int32 BulletsPerLine = FMath::Max(2, Params.Count / 5);
	FVector Origin = Params.Origin;

	// 1. Calculamos la dirección real desde el ataque hacia el jugador
	FVector DirectionToPlayer = (PlayerPawn->GetActorLocation() - Origin).GetSafeNormal();

	// 2. Anulamos el eje Z temporalmente para que la partitura no se incline 
	// si el jugador está saltando o agachado. Queremos que el muro esté perfectamente derecho.
	FVector ForwardDir = DirectionToPlayer;
	ForwardDir.Z = 0.0f;
	ForwardDir.Normalize();

	// 3. Calculamos la Izquierda/Derecha usando Producto Cruz con el eje Z (Arriba) universal.
	// Esto nos da un vector que cruza exactamente de forma perpendicular a la mirada hacia el jugador.
	FVector UpDir = FVector::UpVector;
	FVector RightDir = FVector::CrossProduct(UpDir, ForwardDir).GetSafeNormal();

	float HalfWidth = StaffWidth / 2.0f;
	float StartUpOffset = -2.0f * LineSpacing;

	for (int32 LineIndex = 0; LineIndex < 5; LineIndex++)
	{
		// Calculamos la ALTURA de esta línea
		float CurrentUpOffset = StartUpOffset + (LineIndex * LineSpacing);
		FVector LineCenter = Origin + (UpDir * CurrentUpOffset);

		// Calculamos los extremos de la línea usando nuestro nuevo RightDir dinámico
		FVector LineStart = LineCenter - (RightDir * HalfWidth);
		FVector LineEnd = LineCenter + (RightDir * HalfWidth);

		for (int32 BulletIndex = 0; BulletIndex < BulletsPerLine; BulletIndex++)
		{
			float Alpha = (float)BulletIndex / (float)(BulletsPerLine - 1);
			FVector CalculatedSpawnLoc = FMath::Lerp(LineStart, LineEnd, Alpha);

			// Todo el pentagrama avanza como un muro hacia el jugador
			FVector Direction = ForwardDir;

			Spawner->InternalSpawn(CalculatedSpawnLoc, Direction, Params.Speed, Params.Damage, Params.Scale);
		}
	}
}