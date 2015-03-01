

#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AmethystAIController.generated.h"

/**
 * 
 */
UCLASS(config=Game) 
class AMETHYSTFOREST_API AAmethystAIController : public AAIController
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(transient)
		class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
		class UBehaviorTreeComponent* BehaviorComp;

	virtual void Possess(class APawn* InPawn) override;

	virtual void BeginInactiveState() override;

	void Respawn();


	UFUNCTION(BlueprintCallable, Category = Behavior)
		void SetEnemy(class APawn* InPawn);
	UFUNCTION(BlueprintCallable, Category = Behavior)
	class AAmethystCharacter* GetEnemy() const;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void CheckAmmo(const class AAmethystWeapon* CurrentWeapon);

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		bool PawnCanBeSeen(APawn * target);

	/* Checks sight to all pawns in map, sets enemy if it finds a thing */

	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void SearchEnemyInView();
	UFUNCTION(BlueprintCallable, Category = Behaviour)
		void SetEnemyLastKnownLocation(class APawn* InPawn);

	void UpdateControlRotation(float DeltaTime, bool bUpdatePawn);

	void FindClosestEnemy();

protected:
	int32 EnemyKeyID;
	int32 NeedAmmoKeyID;

	/** Handle for efficient management of Respawn timer */
	FTimerHandle TimerHandle_Respawn;
	
	int32 EnemyLastKnownLocation;
	
	
};
	
	

