

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "AmethystCharMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class AMETHYSTFOREST_API UAmethystCharMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual float GetMaxSpeedModifier() const OVERRIDE;
	
};
