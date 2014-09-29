

#include "amethystforest.h"
#include "amethystforestGameMode.h"
#include "amethystforestPlayerController.h"

AamethystforestGameMode::AamethystforestGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PlayerControllerClass = AamethystforestPlayerController::StaticClass();
}


