#include "Subsystems/ProjectilesSubsystem.h"
#include "GameFramework/Actor.h"
#include "Player/PlayerStatsInterface.h"
#include "Components/WeakPointComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Core/Orchestrator/OrchestratorGameMode.h"
#include "Components/BulletSpawnerComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

float UProjectilesSubsystem::GetPlayerProjectileSpeedMultiplier(AActor* OwnerActor)
{
    if (!OwnerActor) return 1.f;
    // Try to get player stats interface from owner actor
    if (OwnerActor->GetClass()->ImplementsInterface(UPlayerStatsInterface::StaticClass()))
    {
        IPlayerStatsInterface* PSI = Cast<IPlayerStatsInterface>(OwnerActor);
        if (PSI) return PSI->GetProjectileSpeedMultiplier();
    }
    // fallback: try to find buff component and compute multipliers (not implemented here)
    return 1.f;
}

bool UProjectilesSubsystem::Tick(float DeltaTime)
{
	if (!bIsActive) return true;

	if (GetWorld() && GetWorld()->IsPaused()) return true;

	if (BulletPool.Num() == 0) InitializePool();

	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && Bullet->BulletData.bIsActive)
		{
			FVector CurrentLoc = Bullet->GetActorLocation();

			if (Bullet->BulletData.ConvergeDelay > 0.f)
			{
				Bullet->BulletData.ConvergeDelay -= DeltaTime;
				if (Bullet->BulletData.ConvergeDelay <= 0.f)
				{
					FVector Dir = (Bullet->BulletData.ConvergeCenter - CurrentLoc).GetSafeNormal();
					Bullet->BulletData.Direction = Dir;
					Bullet->BulletData.Speed = 600.f;
				}
				Bullet->SetActorLocation(CurrentLoc, true);
				continue;
			}
			// FVector NewLoc = CurrentLoc + (Bullet->BulletData.Direction * Bullet->BulletData.Speed * DeltaTime);

			FVector NewLoc = CurrentLoc + (Bullet->BulletData.Direction * Bullet->BulletData.Speed * GlobalSpeedMultiplier * DeltaTime);


			FCollisionQueryParams Params;
			Params.AddIgnoredActor(Bullet);
			//if (Bullet->BulletData.OwnerActor) Params.AddIgnoredActor(Bullet->BulletData.OwnerActor);



			// DETECCI?N DE COLISI?N LIGERA
			FHitResult Hit;
			//FCollisionQueryParams Params;
			//Params.AddIgnoredActor(Bullet);

			if (Bullet->BulletData.OwnerActor)
			{
				Params.AddIgnoredActor(Bullet->BulletData.OwnerActor);
			}

			for (ABulletBase* OtherBullet : BulletPool)
			{
				if (OtherBullet && OtherBullet != Bullet && OtherBullet->BulletData.bIsActive)
				{
					Params.AddIgnoredActor(OtherBullet);
				}
			}

			if (GetWorld()->LineTraceSingleByChannel(Hit, CurrentLoc, NewLoc, ECC_Visibility, Params))
			{
				AActor* OtherActor = Hit.GetActor();

				if (OtherActor && OtherActor != Bullet)
				{
					bool bIsBoss = OtherActor->ActorHasTag("Jefe");
					bool bIsEnemy = bIsBoss || OtherActor->ActorHasTag("Enemigo");
					bool bIsPlayer = OtherActor->ActorHasTag("Player");

					// Player bullet hits enemy/boss
					if (Bullet->BulletData.bIsPlayerBullet && bIsEnemy)
					{
						if (bIsBoss)
						{
							UWeakPointComponent* HitWP = Cast<UWeakPointComponent>(Hit.GetComponent());
							if (HitWP)
							{
								if (HitWP->GetGenerateOverlapEvents() && HitWP->CurrentHealth > 0.0f)
								{
									HitWP->TakeDamageFromHit(Bullet->BulletData.Damage);
								}
								ReturnBullet(Bullet);
								continue;
							}

							bool bHasActiveWeakPoints = false;
							TArray<UWeakPointComponent*> WPs;
							OtherActor->GetComponents<UWeakPointComponent>(WPs);
							for (UWeakPointComponent* WP : WPs)
							{
								if (WP->CurrentHealth > 0.0f)
								{
									bHasActiveWeakPoints = true;
									break;
								}
							}

							if (bHasActiveWeakPoints)
							{
								ReturnBullet(Bullet);
								continue;
							}
							else
							{
								UGameplayStatics::ApplyDamage(OtherActor, Bullet->BulletData.Damage, nullptr, Bullet, UDamageType::StaticClass());
								ReturnBullet(Bullet);
								continue;
							}
						}
						else
						{
							UGameplayStatics::ApplyDamage(OtherActor, Bullet->BulletData.Damage, nullptr, Bullet, UDamageType::StaticClass());
							ReturnBullet(Bullet);
							continue;
						}
					}

					// Si la bala es del jefe y le pega al jugador...
					if (!Bullet->BulletData.bIsPlayerBullet && bIsPlayer)
					{
						UGameplayStatics::ApplyDamage(OtherActor, Bullet->BulletData.Damage, nullptr, Bullet, UDamageType::StaticClass());
						ReturnBullet(Bullet);
						continue;
					}

					// Si choca con una pared (sin tags), tambi?n se recicla
					if (!bIsEnemy && !bIsPlayer)
					{
						ReturnBullet(Bullet);
						continue;
					}
				}
			}

			// RECICLAJE POR VIDA LIMITADA (Secret Level: player bullets last 1s)
			if (Bullet->BulletData.RemainingLifetime > 0.f)
			{
				Bullet->BulletData.RemainingLifetime -= DeltaTime;
				if (Bullet->BulletData.RemainingLifetime <= 0.f)
				{
					ReturnBullet(Bullet);
					continue;
				}
			}

			// MOVIMIENTO (sweep=true para activar overlap events en WeakPointComponent)
			Bullet->SetActorLocation(NewLoc, true);

			// RECICLAJE POR DISTANCIA (L?mite de  unidades)
			if (FVector::Dist(NewLoc, Bullet->BulletData.SpawnLocation) > 4000.f)
			{
				ReturnBullet(Bullet);
			}
		}
	}
	return true;
}

void UProjectilesSubsystem::InitializePool()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	for (int32 i = 0; i < PoolSize; i++)
	{
		ABulletBase* NewBullet = World->SpawnActor<ABulletBase>(ABulletBase::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewBullet) BulletPool.Add(NewBullet);
	}
}

ABulletBase* UProjectilesSubsystem::RequestBullet(FVector Loc, FVector Dir, float Spd, bool bIsPlayer, float Damage, FVector SpawnLocation, AActor* Owner, FVector Scale)
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && !Bullet->BulletData.bIsActive)
		{
			Bullet->ActivateBullet(SpawnLocation, Dir, Spd, bIsPlayer, Damage, SpawnLocation, Owner);

			if (Scale != FVector(0.4f))
			{
				Bullet->SetActorRelativeScale3D(Scale);
			}
			return Bullet;
		}
	}
	return nullptr;
}

void UProjectilesSubsystem::ReturnBullet(ABulletBase* Bullet)
{
	if (!Bullet)	return;
	// C�digo del ataque esferico para no interrumpir el ataque del jefe...
	if (!Bullet->BulletData.bIsPlayerBullet && Bullet->BulletData.bIsActive && Bullet->GetActorRelativeScale3D().X > 2.0f)
	{
		if (Bullet->BulletData.OwnerActor)
		{
			// Buscamos el Spawner del Jefe
			UBulletSpawnerComponent* Spawner = Bullet->BulletData.OwnerActor->FindComponentByClass<UBulletSpawnerComponent>();

			if (Spawner)
			{
				FAttackStep ExplosionStep(EAttackType::Sphere, 24, 1200.0f, 0.0f, Bullet->GetActorLocation(), 0.0f, 0.0f, 5.0f);
				ExplosionStep.BulletScale = FVector(0.4f); // Queremos que las esquirlas sean peque�as

				Spawner->ExecuteSingleAttack(ExplosionStep);
			}
		}
	}
	//--------------------------------------------

	Bullet->DesactivateBullet();
}

void UProjectilesSubsystem::HandleBeatHit(bool bIsStrongBeat)
{
	GlobalSpeedMultiplier = 1.0f;
	if (RhythmMPC && GetWorld())
	{
		float ScaleTarget = bIsStrongBeat ? 2.0f : 1.25f;
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), RhythmMPC, FName("BeatPulseScale"), ScaleTarget);
	}
}

void UProjectilesSubsystem::HandleSilenceEnter()
{
	GlobalSpeedMultiplier = 0.0f;
	if (RhythmMPC && GetWorld())
	{
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), RhythmMPC, FName("BeatPulseScale"), 1.0f);
	}
}

void UProjectilesSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsActive = true;

	TickHandle = FTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UProjectilesSubsystem::Tick));

	OnMapLoadedHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda(
		[this](UWorld* LoadedWorld)
		{
			ClearPool();
			ReinitializePool();
		});
}

void UProjectilesSubsystem::Deinitialize()
{
	bIsActive = false;

	if (TickHandle.IsValid())
	{
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(OnMapLoadedHandle);
	ClearPool();
	Super::Deinitialize();
}

void UProjectilesSubsystem::ClearPool()
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && IsValid(Bullet))
			Bullet->Destroy();
	}
	BulletPool.Empty();
}

void UProjectilesSubsystem::ReinitializePool()
{
	InitializePool();
	UE_LOG(LogTemp, Warning, TEXT("[ProjectilesSubsystem] Pool reinicializado con %d balas."), BulletPool.Num());
}


void UProjectilesSubsystem::ReturnAllActiveBullets()
{
	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && Bullet->BulletData.bIsActive)
		{
			ReturnBullet(Bullet);
		}
	}
}

void UProjectilesSubsystem::RedirectAllBossBulletsToTarget(FVector TargetLocation, float NewSpeed)
{
	TArray<ABulletBase*> BulletsToExplode;

	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && Bullet->BulletData.bIsActive && !Bullet->BulletData.bIsPlayerBullet)
		{
			if (Bullet->GetActorRelativeScale3D().X > 2.0f)
			{
				// La marcamos para explotar
				BulletsToExplode.Add(Bullet);
			}
			else
			{
				// Calculamos el nuevo vector de direcci�n directo al objetivo (el jugador)
				FVector NewDir = (TargetLocation - Bullet->GetActorLocation()).GetSafeNormal();
				Bullet->BulletData.Direction = NewDir;
				Bullet->BulletData.Speed = NewSpeed;
			}
		}
	}
	for (ABulletBase* Bullet : BulletsToExplode)
	{
		ReturnBullet(Bullet);
	}
}

void UProjectilesSubsystem::ExecuteSilenceCollapse(FVector TargetLocation, float CollapseSpeed)
{
	// 1. Descongelamos el universo para que las balas puedan moverse
	GlobalSpeedMultiplier = 1.0f;

	// 2. Obligamos a las balas a apuntar al jugador con la velocidad extrema
	for (ABulletBase* Bullet : BulletPool)
	{
		if (Bullet && Bullet->BulletData.bIsActive && !Bullet->BulletData.bIsPlayerBullet)
		{
			// Calculamos direcci�n
			FVector Dir = (TargetLocation - Bullet->GetActorLocation()).GetSafeNormal();
			Bullet->BulletData.Direction = Dir;

			// Asignamos la velocidad supers�nica
			Bullet->BulletData.Speed = CollapseSpeed;

			// �NO tocamos ConvergeDelay! As� el Tick() no nos bajar� la velocidad a 600.f
		}
	}
}