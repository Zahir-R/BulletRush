// Fill out your copyright notice in the Description page of Project Settings.


#include "VaultKeeper/objets/BatteryActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Components/BulletSpawnerComponent.h"
#include "VaultKeeper/enemies/DronMecha.h"
#include "Engine/World.h"

ABatteryActor::ABatteryActor()
{
    PrimaryActorTick.bCanEverTick = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
    BatteryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BatteryMesh"));
    RootComponent = BatteryMesh;
    if (MeshAsset.Succeeded())
    {
        BatteryMesh->SetStaticMesh(MeshAsset.Object);
    }


    HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
    HealthComp->MaxHealth = 150.0f;

    Tags.Add(FName("Enemy"));
}

void ABatteryActor::BeginPlay()
{
    Super::BeginPlay();

    HealthComp->OnDeath.AddDynamic(this, &ABatteryActor::OnDeath);


    // Curación cada segundo
    GetWorld()->GetTimerManager().SetTimer(
        HealLoop,
        this,
        &ABatteryActor::ApplyHeal,
        1.0f,
        true
    );
}

float ABatteryActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float RealDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    HealthComp->TakeDamage(RealDamage, DamageEvent, EventInstigator, DamageCauser);
    return RealDamage;
}

void ABatteryActor::LinkDrone(ADronMecha* Drone)
{
    LinkedDrone = Drone;
    Subscribe(Cast<AActor>(Drone)); // APublisher::Subscribe
}

void ABatteryActor::ApplyHeal()
{
    if (LinkedDrone && IsValid(LinkedDrone))
    {
        LinkedDrone->HealthComp->Heal(HealRate);
    }
}

void ABatteryActor::OnDeath()
{
    Dead();
}

void ABatteryActor::Dead()
{
    UBulletSpawnerComponent* Spawner = FindComponentByClass<UBulletSpawnerComponent>();
    if (!Spawner) return;
    // Detenemos curación
    GetWorld()->GetTimerManager().ClearTimer(HealLoop);

    // Notificamos al drone que perdió su batería
    NotifySubscribers();

    UE_LOG(LogTemp, Warning, TEXT("[BatteryActor] Destruida, drone sin curacion"));
    //disparar una esfera de balas 
    TArray<FAttackStep> Pattern;
    Pattern.Add(FAttackStep(EAttackType::Sphere, 500, 800.0f, 0.5f, 0.1f));
    Spawner->StartSequence(Pattern);

    Destroy();
}

