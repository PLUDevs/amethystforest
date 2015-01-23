

#include "amethystforest.h"
#include "../../Classes/Bots/AmethystAIController.h"
#include "../../Classes/Player/AmethystCharacter.h"
#include "../../Classes/Weapon/AmethystWeapon.h"
#include "../../Classes/Bots/AmethystBot.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


AAmethystAIController::AAmethystAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP) 
{
	// create blackboard and behaviour components in the constructor
	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	bWantsPlayerState = true;
}



void AAmethystAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	AAmethystBot* Bot = Cast<AAmethystBot>(InPawn);

	// start behavior
	if (Bot && Bot->BotBehavior)
	{

		BlackboardComp->InitializeBlackboard(Bot->BotBehavior->BlackboardAsset);

		// Get the enemy blackboard ID, and store it to access that blackboard key later.
		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		NeedAmmoKeyID = BlackboardComp->GetKeyID("NeedAmmo");


		BehaviorComp->StartTree(Bot->BotBehavior);
	}
}

void AAmethystAIController::BeginInactiveState()
{
	Super::BeginInactiveState();

	AGameState* GameState = GetWorld()->GameState;

	const float MinRespawnDelay = (GameState && GameState->GameModeClass) ? GetDefault<AGameMode>(GameState->GameModeClass)->MinRespawnDelay : 1.0f;

	GetWorldTimerManager().SetTimer(this, &AAmethystAIController::Respawn, MinRespawnDelay);
}

void AAmethystAIController::Respawn()
{
	//	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}




void AAmethystAIController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

class AAmethystCharacter* AAmethystAIController::GetEnemy() const
{
	if (BlackboardComp)
	{
		return Cast<AAmethystCharacter>(BlackboardComp->GetValueAsObject(EnemyKeyID));
	}

	return NULL;
}


void AAmethystAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	// Look toward focus
	FVector FocalPoint = GetFocalPoint();
	if (!FocalPoint.IsZero() && GetPawn())
	{
		FVector Direction = FocalPoint - GetPawn()->GetActorLocation();
		FRotator NewControlRotation = Direction.Rotation();

		NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);

		SetControlRotation(NewControlRotation);

		APawn* const P = GetPawn();
		if (P && bUpdatePawn)
		{
			P->FaceRotation(NewControlRotation, DeltaTime);
		}

	}
}

void AAmethystAIController::CheckAmmo(const class AAmethystWeapon* CurrentWeapon)
{
	if (CurrentWeapon && BlackboardComp)
	{
		const int32 Ammo = CurrentWeapon->GetCurrentAmmo();
		const int32 MaxAmmo = CurrentWeapon->GetMaxAmmo();
		const float Ratio = (float)Ammo / (float)MaxAmmo;

		BlackboardComp->SetValueAsBool(NeedAmmoKeyID, (Ratio <= 0.1f));
	}
}

void AAmethystAIController::FindClosestEnemy()
{
	/* Need base character classes 
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	AAmethystCharacter* BestPawn = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		AAmethystCharacter* TestPawn = Cast<AAmethystCharacter>(*It);
		if (TestPawn && TestPawn->IsAlive() && TestPawn->IsEnemyFor(this))
		{
			const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestPawn = TestPawn;
			}
		}
	}

	if (BestPawn)
	{
		SetEnemy(BestPawn);
	}
	*/
}
bool AAmethystAIController::PawnCanBeSeen(APawn * target)
{
	if (target == NULL || GetPawn() == NULL)
	{
		return false;
	}
	FVector difference = target->GetActorLocation() - GetPawn()->GetActorLocation();
	float angle = FVector::DotProduct(difference, GetPawn()->GetActorRotation().Vector());

	if (LineOfSightTo(target, GetPawn()->GetActorLocation()) && angle >0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void AAmethystAIController::SearchEnemyInView()
{
	APawn* MyBot = GetPawn();
	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	AAmethystCharacter* BestPawn = NULL;

	//foreach all pawns in world
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		//UE_LOG(LogShooterWeapon, Log, TEXT(" ENEMY SEEN %s "), *GetNameSafe(*It));
		if (PawnCanBeSeen(*It))
		{
			AAmethystCharacter* TestPawn = Cast<AAmethystBot>(*It);

			if (TestPawn && TestPawn->IsAlive() && Cast<AAmethystCharacter>(TestPawn) == NULL)
			{
				const float DistSq = (TestPawn->GetActorLocation() - MyLoc).SizeSquared();
				if (DistSq < BestDistSq)
				{
					BestDistSq = DistSq;
					BestPawn = TestPawn;
				}
			}
		}
	}

	if (BestPawn)
	{
		// We saw someone, so set him as target.
		SetEnemy(BestPawn);

	}

}




