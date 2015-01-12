

#pragma once

#include "GameFramework/GameMode.h"
#include "amethystforestGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AMETHYSTFOREST_API AamethystforestGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

    /** prevents friendly fire */
    virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	
};
