// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Subsystems/ProjectilesSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void UProjectilesSubsystem::Tick(float DeltaTime)
{
	if (BulletPool.Num() == 0) InitializePool();

	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && Bullet->BulletData.bIsActive)
		{
			FVector CurrentLoc = Bullet->GetActorLocation();
			FVector NewLoc = CurrentLoc + (Bullet->BulletData.Direction * Bullet->BulletData.Speed * DeltaTime);

			// DETECCIÓN DE COLISIÓN LIGERA
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Bullet);
			
			if (Bullet->BulletData.OwnerActor)
			{
				Params.AddIgnoredActor(Bullet->BulletData.OwnerActor);
			}

			if (GetWorld()->LineTraceSingleByChannel(Hit, CurrentLoc, NewLoc, ECC_Visibility, Params))
			{
				AActor* OtherActor = Hit.GetActor();

			if (OtherActor && OtherActor != Bullet) // Verificamos que no sea la propia bala
				{
					// 1. LÓGICA DE FACCIONES (Contexto que hablamos)
					bool bIsEnemy = OtherActor->ActorHasTag("Jefe") || OtherActor->ActorHasTag("Enemigo");
					bool bIsPlayer = OtherActor->ActorHasTag("Player");

					// 2. ¿QUIÉN LE PEGA A QUIÉN?
					// Si la bala es del jugador y le pega a un enemigo...
					if (Bullet->BulletData.bIsPlayerBullet && bIsEnemy)
					{
						UGameplayStatics::ApplyDamage(OtherActor, Bullet->BulletData.Damage, nullptr, Bullet, UDamageType::StaticClass());
						ReturnBullet(Bullet); // RECICLAMOS, NO DESTRUIMOS
						continue;
					}

					// Si la bala es del jefe y le pega al jugador...
					if (!Bullet->BulletData.bIsPlayerBullet && bIsPlayer)
					{
						UGameplayStatics::ApplyDamage(OtherActor, Bullet->BulletData.Damage, nullptr, Bullet, UDamageType::StaticClass());
						ReturnBullet(Bullet);
						continue;
					}

					// Si choca con una pared (sin tags), también se recicla
					if (!bIsEnemy && !bIsPlayer)
					{
						ReturnBullet(Bullet);
						continue;
					}
				}
			}

			// MOVIMIENTO
			Bullet->SetActorLocation(NewLoc);

			// RECICLAJE POR DISTANCIA (Límite de 8000 unidades)
			if (FVector::Dist(NewLoc,Bullet->BulletData.SpawnLocation) > 8000.f)
			{
				ReturnBullet(Bullet);
			}
		}
	}
}

void UProjectilesSubsystem::InitializePool()
{
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	for (int32 i = 0; i < PoolSize; i++)
	{
		ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(ABulletBase::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewBullet) BulletPool.Add(NewBullet);
	}
}

ABulletBase* UProjectilesSubsystem::RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner)
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && !Bullet->BulletData.bIsActive)
		{
			Bullet->ActivateBullet(SpawnLocation, Dir, Spd, bIsPlayer, Damage, SpawnLocation, Owner);
			return Bullet;
		}
	}
	return nullptr;
}

void UProjectilesSubsystem::ReturnBullet(ABulletBase* Bullet)
{
	if (Bullet) Bullet->DesactivateBullet();
}