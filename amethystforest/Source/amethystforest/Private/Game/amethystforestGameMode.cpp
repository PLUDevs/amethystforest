

#include "amethystforest.h"
#include "../../Classes/Game/amethystforestGameMode.h"
#include "../../Classes/Player/amethystforestPlayerController.h"

AamethystforestGameMode::AamethystforestGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
		//PlayerControllerClass = AamethystforestPlayerController::StaticClass();
			// set default pawn class to our Blueprinted character
		static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/AmethystCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	
			// use our custom HUD class
			//HUDClass = AMyProjectHUD::StaticClass();
}


