#include "Enemies/Chronostasis/Boss/AlteredZone.h"
#include "Combat/BulletBase.h"
#include "Engine/World.h"

AAlteredZone::AAlteredZone()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	SphereCollision->InitSphereRadius(ZoneRadius);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);
}

void AAlteredZone::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AAlteredZone::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AAlteredZone::OnOverlapEnd);

	GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &AAlteredZone::OnLifetimeExpired, ZoneLifetime, false);
}

void AAlteredZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABulletBase* Bullet = Cast<ABulletBase>(OtherActor);
	if (Bullet && Bullet->BulletData.bIsActive && !OriginalSpeeds.Contains(Bullet))
	{
		OriginalSpeeds.Add(Bullet, Bullet->BulletData.Speed);
		if (Bullet->BulletData.bIsPlayerBullet)
			Bullet->BulletData.Speed = Bullet->BulletData.Speed * SlowSpeed;
		else
			Bullet->BulletData.Speed = Bullet->BulletData.Speed + SpeedUp;
	}
}

void AAlteredZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABulletBase* Bullet = Cast<ABulletBase>(OtherActor);
	if (Bullet && OriginalSpeeds.Contains(Bullet))
	{
		float Original = OriginalSpeeds[Bullet];
		float Expected = Bullet->BulletData.bIsPlayerBullet
			? Original * SlowSpeed
			: Original + SpeedUp;
		if (FMath::IsNearlyEqual(Bullet->BulletData.Speed, Expected, 1.0f))
		{
			Bullet->BulletData.Speed = Original;
		}
		OriginalSpeeds.Remove(Bullet);
	}
}

void AAlteredZone::OnLifetimeExpired()
{
	for (auto& Pair : OriginalSpeeds)
	{
		if (Pair.Key && IsValid(Pair.Key) && Pair.Key->BulletData.bIsActive)
		{
			float Expected = Pair.Key->BulletData.bIsPlayerBullet
				? Pair.Value * SlowSpeed
				: Pair.Value + SpeedUp;
			if (FMath::IsNearlyEqual(Pair.Key->BulletData.Speed, Expected, 1.0f))
			{
				Pair.Key->BulletData.Speed = Pair.Value;
			}
		}
	}
	OriginalSpeeds.Empty();
	Destroy();
}
