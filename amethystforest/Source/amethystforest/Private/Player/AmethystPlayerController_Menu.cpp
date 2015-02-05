
#include "amethystforest.h"
#include "Classes/Player/AmethystPlayerController_Menu.h"
#include "Private/UI/Style/AmethystStyle.h"


AAmethystPlayerController_Menu::AAmethystPlayerController_Menu(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
}

void AAmethystPlayerController_Menu::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    FAmethystStyle::Initialize();
}
