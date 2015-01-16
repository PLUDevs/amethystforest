

#pragma once

#include "Bots/AmethystAIController.h"

#include "Player/AmethystCharacter.h"
#include "AmethystAIControllerRanged.generated.h"

/**
 * 
 */
UCLASS()
class AMETHYSTFOREST_API AAmethystAIControllerRanged : public AAmethystAIController
{
	GENERATED_UCLASS_BODY()
		//TODO add check for range
	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool HasWeaponLOSToEnemy(AActor* InEnemyActor, const bool bAnyEnemy) const;
	/* If there is line of sight to current enemy, start firing at them */
	UFUNCTION(BlueprintCallable, Category = Behavior)
		void ShootEnemy();
		void CheckAmmo(const class AShooterWeapon* CurrentWeapon);
protected:
	// Check of we have LOS to a character
	bool LOSTrace(AAmethystCharacter* InEnemyChar) const;

	
	int32 NeedAmmoKeyID;
};
