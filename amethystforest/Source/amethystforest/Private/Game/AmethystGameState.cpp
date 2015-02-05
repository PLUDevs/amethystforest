
#include "amethystforest.h"
#include "Classes/Game/AmethystGameState.h"
#include "Classes/Game/amethystforestGameMode.h"

AAmethystGameState::AAmethystGameState(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
}

void AAmethystGameState::RequestFinishAndExitToMainMenu()
{
    if (AuthorityGameMode)
    {
        // we are server, tell the gamemode
        AamethystforestGameMode* const GameMode = Cast<AamethystforestGameMode>(AuthorityGameMode);
        if (GameMode)
        {
            GameMode->RequestFinishAndExitToMainMenu();
        }
    }
    else
    {
        APlayerController* const PrimaryPC = GetGameInstance()->GetFirstLocalPlayerController();
        if (PrimaryPC)
        {
            check(PrimaryPC->GetNetMode() == ENetMode::NM_Client);
            PrimaryPC->ClientReturnToMainMenu(TEXT("Return to Main Menu requested by game state"));
        }
    }
    
}
