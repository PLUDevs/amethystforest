

#include "amethystforest.h"
#include "../../Classes/Game/amethystforestGameMode.h"
#include "../../Classes/Player/amethystforestPlayerController.h"

AamethystforestGameMode::AamethystforestGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PlayerControllerClass = AamethystforestPlayerController::StaticClass();
}


