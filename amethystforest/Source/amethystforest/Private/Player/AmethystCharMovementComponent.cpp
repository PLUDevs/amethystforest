

#include "amethystforest.h"
#include "Classes/Player/AmethystCharMovementComponent.h"
#include "Classes/Player/AmethystCharacter.h"


UAmethystCharMovementComponent::UAmethystCharMovementComponent(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

float UAmethystCharMovementComponent::GetMaxSpeed() const
{
	float SpeedMod = Super::GetMaxSpeed();

	const AAmethystCharacter* CharacterOwner = Cast<AAmethystCharacter>(PawnOwner);
	if (CharacterOwner)
	{
		if (CharacterOwner->IsTargeting())
		{
			SpeedMod *= CharacterOwner->GetTargetingSpeedModifier();
		}
		if (CharacterOwner->IsRunning())
		{
			SpeedMod *= CharacterOwner->GetRunningSpeedModifier();
		}
	}

	return SpeedMod;
}
