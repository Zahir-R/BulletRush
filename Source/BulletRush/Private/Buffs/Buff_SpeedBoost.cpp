#include "Buffs/Buff_SpeedBoost.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UBuff_SpeedBoost::Apply(AActor* InOwner)
{
	Super::Apply(InOwner);
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			OriginalSpeed = MovementComp->MaxFlySpeed;
			MovementComp->MaxFlySpeed *= SpeedMultiplier;
		}
	}
}

void UBuff_SpeedBoost::Remove()
{
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			MovementComp->MaxFlySpeed = OriginalSpeed;
		
	}
	Super::Remove();
}