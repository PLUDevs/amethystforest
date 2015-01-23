

#pragma once

#include "../../Classes/Player/AmethystCharacter.h"
#include "AmethystBot.generated.h"

/**
 * 
 */
UCLASS()
class AMETHYSTFOREST_API AAmethystBot : public AAmethystCharacter
{
	GENERATED_UCLASS_BODY()


		UPROPERTY(EditDefaultsOnly, Category = Behaviour)
		float AttackRange;
	UPROPERTY(EditDefaultsOnly, Category = Behaviour)
		float AttackDamage;

	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	UFUNCTION(BlueprintCallable, Category = Behavior)
		void PerformMeleeAttack();

	float AccumulatedFiretime;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) OVERRIDE;
	virtual bool IsFirstPerson() const OVERRIDE;


	virtual void ReceiveHit
		(
	class UPrimitiveComponent * MyComp,
	class AActor * Other,
	class UPrimitiveComponent * OtherComp,
		bool bSelfMoved,
		FVector HitLocation,
		FVector HitNormal,
		FVector NormalImpulse,
		const FHitResult & Hit
		) OVERRIDE;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) OVERRIDE;
	bool Attacking;
};