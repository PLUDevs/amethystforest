

#include "amethystforest.h"
#include "../../Classes/Player/AmethystCharMovementComponent.h"
#include "../../Classes/Player/AmethystCharacter.h"


UAmethystCharMovementComponent::UAmethystCharMovementComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

float UAmethystCharMovementComponent::GetMaxSpeedModifier() const
{
	float SpeedMod = Super::GetMaxSpeedModifier();

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
