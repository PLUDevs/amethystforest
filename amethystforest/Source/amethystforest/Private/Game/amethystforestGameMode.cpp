

#include "amethystforest.h"
#include "../../Classes/Game/amethystforestGameMode.h"
#include "../../Classes/Player/amethystforestPlayerController.h"

AamethystforestGameMode::AamethystforestGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
		PlayerControllerClass = AamethystforestPlayerController::StaticClass();
			// set default pawn class to our Blueprinted character
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/AmethystCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	
			// use our custom HUD class
			//HUDClass = AMyProjectHUD::StaticClass();
}

float AamethystforestGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
    float ActualDamage = Damage;
    
    AAmethystCharacter* DamagedPawn = Cast<AAmethystCharacter>(DamagedActor);
    if (DamagedPawn && EventInstigator)
    {
        /* TO DO: PlayerState
        AAmethystPlayerState* DamagedPlayerState = Cast<AAmethystPlayerState>(DamagedPawn->PlayerState);
        AAmethystPlayerState* InstigatorPlayerState = Cast<AAmethystPlayerState>(EventInstigator->PlayerState);
        
        // disable friendly fire
        if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
        {
            ActualDamage = 0.0f;
        }
        
        // scale self instigated damage
        if (InstigatorPlayerState == DamagedPlayerState)
        {
            ActualDamage *= DamageSelfScale;
        } */
        return 0; //Remove later
    }
    
    return ActualDamage;
}

