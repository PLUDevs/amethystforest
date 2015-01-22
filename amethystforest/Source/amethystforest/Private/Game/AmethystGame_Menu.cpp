

#include "amethystforest.h"
#include "../../Classes/Game/AmethystGame_Menu.h"
#include "../../Classes/Player/amethystforestPlayerController.h"
#include "../UI/Menu/AmethystMainMenu.h"
#include "../UI/Menu/AmethystMainMenu.h"
#include "../UI/Menu/AmethystWelcomeMenu.h"

AAmethystGame_Menu::AAmethystGame_Menu(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    PlayerControllerClass = AamethystforestPlayerController::StaticClass();
}

void AAmethystGame_Menu::RestartPlayer(class AController* NewPlayer)
{
    // don't restart
}

/** Returns game session class to use */
/* TO DO: GameSession Class, don't think we need.
TSubclassOf<AGameSession> AAmethystGame_Menu::GetGameSessionClass() const
{

    return AAmethystGameSession::StaticClass();
    
}
 */
