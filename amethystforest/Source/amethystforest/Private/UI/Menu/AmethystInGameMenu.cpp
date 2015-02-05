
#include "amethystforest.h"
#include "Classes/Player/amethystforestPlayerController.h"
#include "AmethystIngameMenu.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Style/AmethystMenuSoundsWidgetStyle.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

void FAmethystIngameMenu::Construct(ULocalPlayer* _PlayerOwner)
{
    PlayerOwner = _PlayerOwner;
    bIsGameMenuUp = false;
    
    if (!GEngine || !GEngine->GameViewport)
    {
        return;
    }
    
    if (!GameMenuWidget.IsValid())
    {
        SAssignNew(GameMenuWidget, SAmethystMenuWidget)
        .PlayerOwner(TWeakObjectPtr<ULocalPlayer>(PlayerOwner))
        .Cursor(EMouseCursor::Default)
        .IsGameMenu(true);
        
        
        // setup the exit to main menu submenu.  We wanted a confirmation to avoid a potential TRC violation.
        // fixes TTP: 322267
        TSharedPtr<FAmethystMenuItem> MainMenuRoot = FAmethystMenuItem::CreateRoot();
        MainMenuItem = MenuHelper::AddMenuItem(MainMenuRoot,LOCTEXT("Main Menu", "MAIN MENU"));
        MenuHelper::AddMenuItemSP(MainMenuItem,LOCTEXT("No", "NO"), this, &FAmethystIngameMenu::OnCancelExitToMain);
        MenuHelper::AddMenuItemSP(MainMenuItem,LOCTEXT("Yes", "YES"), this, &FAmethystIngameMenu::OnConfirmExitToMain);
        
        AmethystOptions = MakeShareable(new FAmethystOptions());
        AmethystOptions->Construct(PlayerOwner);
        AmethystOptions->TellInputAboutKeybindings();
        AmethystOptions->OnApplyChanges.BindSP(this, &FAmethystIngameMenu::CloseSubMenu);
        
        /** root menu item pointer */
        MenuHelper::AddExistingMenuItem(RootMenuItem, AmethystOptions->CheatsItem.ToSharedRef());
        MenuHelper::AddExistingMenuItem(RootMenuItem, AmethystOptions->OptionsItem.ToSharedRef());
        
        if(FSlateApplication::Get().SupportsSystemHelp())
        {
            TSharedPtr<FAmethystMenuItem> HelpSubMenu = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Help", "HELP"));
            HelpSubMenu->OnConfirmMenuItem.BindStatic([](){ FSlateApplication::Get().ShowSystemHelp(); });
        }
        
#if Amethyst_CONSOLE_UI
        TSharedPtr<FAmethystMenuItem> ShowInvitesItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Invite Players", "INVITE PLAYERS"));
        ShowInvitesItem->OnConfirmMenuItem.BindRaw(this, &FAmethystIngameMenu::OnShowInviteUI);
#endif
        
        MenuHelper::AddExistingMenuItem(RootMenuItem, MainMenuItem.ToSharedRef());
        
#if !Amethyst_CONSOLE_UI
        MenuHelper::AddMenuItemSP(RootMenuItem, LOCTEXT("Quit", "QUIT"), this, &FAmethystIngameMenu::OnUIQuit);
#endif
        
        GameMenuWidget->MainMenu = GameMenuWidget->CurrentMenu = RootMenuItem->SubMenu;
        GameMenuWidget->OnMenuHidden.BindSP(this,&FAmethystIngameMenu::DetachGameMenu);
        GameMenuWidget->OnToggleMenu.BindSP(this,&FAmethystIngameMenu::ToggleGameMenu);
        GameMenuWidget->OnGoBack.BindSP(this, &FAmethystIngameMenu::OnMenuGoBack);
    }
}

void FAmethystIngameMenu::CloseSubMenu()
{
    GameMenuWidget->MenuGoBack();
}

void FAmethystIngameMenu::OnMenuGoBack(MenuPtr Menu)
{
    // if we are going back from options menu
    if (AmethystOptions->OptionsItem->SubMenu == Menu)
    {
        AmethystOptions->RevertChanges();
    }
}

bool FAmethystIngameMenu::GetIsGameMenuUp() const
{
    return bIsGameMenuUp;
}

void FAmethystIngameMenu::DetachGameMenu()
{
    GEngine->GameViewport->RemoveViewportWidgetContent(GameMenuContainer.ToSharedRef());
    bIsGameMenuUp = false;
    
    AamethystforestPlayerController* const PCOwner = PlayerOwner ? Cast<AamethystforestPlayerController>(PlayerOwner->PlayerController) : nullptr;
    // If the game is over enable the scoreboard
    if (PCOwner)
    {
        PCOwner->SetPause( false );
        AAmethystHUD* const AmethystHUD = PCOwner->GetAmethystHUD();
//        TO DO: What should we show instead?
//        if( ( AmethystHUD != NULL ) && ( AmethystHUD->IsMatchOver() == true ) && ( PCOwner->IsPrimaryPlayer() == true ) )
//        {
//            AmethystHUD->ShowScoreboard( true );
//        }
    }
}

void FAmethystIngameMenu::ToggleGameMenu()
{
    if (!GameMenuWidget.IsValid())
    {
        return;
    }
    
    if (bIsGameMenuUp && GameMenuWidget->CurrentMenu != RootMenuItem->SubMenu)
    {
        GameMenuWidget->MenuGoBack();
        return;
    }
    
    AamethystforestPlayerController* const PCOwner = PlayerOwner ? Cast<AamethystforestPlayerController>(PlayerOwner->PlayerController) : nullptr;
    if (!bIsGameMenuUp)
    {
        // Hide the scoreboard
        if (PCOwner)
        {
            AAmethystHUD* const AmethystHUD = PCOwner->GetAmethystHUD();
//            TO DO: What should we show instead?
//            if( AmethystHUD != NULL )
//            {
//                AmethystHUD->ShowScoreboard( false );
//            }
        }
        
        GEngine->GameViewport->AddViewportWidgetContent(
                                                        SAssignNew(GameMenuContainer,SWeakWidget)
                                                        .PossiblyNullContent(GameMenuWidget.ToSharedRef())
                                                        );
        
        AmethystOptions->UpdateOptions();
        GameMenuWidget->BuildAndShowMenu();
        bIsGameMenuUp = true;
        
        if (PCOwner)
        {
            PCOwner->SetCinematicMode(bIsGameMenuUp, false, false, true, true);
            PCOwner->SetPause(bIsGameMenuUp);
        }
    }
    else
    {
        //Start hiding animation
        GameMenuWidget->HideMenu();
        //enable player controls during hide animation
        FSlateApplication::Get().SetFocusToGameViewport();
        PCOwner->SetCinematicMode(false,false,false,true,true);
    }
}

void FAmethystIngameMenu::OnCancelExitToMain()
{
    CloseSubMenu();
}

void FAmethystIngameMenu::OnConfirmExitToMain()
{
    // tell game instance to go back to main menu state
    UAmethystGameInstance* const GI = Cast<UAmethystGameInstance>(PlayerOwner->GetGameInstance());
    if (GI)
    {
        GI->GotoState(AmethystGameInstanceState::MainMenu);
    }
}

void FAmethystIngameMenu::OnUIQuit()
{
    GameMenuWidget->LockControls(true);
    GameMenuWidget->HideMenu();
    
    UWorld* const World = PlayerOwner ? PlayerOwner->GetWorld() : nullptr;
    if (World)
    {
        const FAmethystMenuSoundsStyle& MenuSounds = FAmethystStyle::Get().GetWidgetStyle<FAmethystMenuSoundsStyle>("DefaultAmethystMenuSoundsStyle");
        MenuHelper::PlaySoundAndCall(World, MenuSounds.ExitGameSound, GetOwnerUserIndex(), this, &FAmethystIngameMenu::Quit);
    }
}

void FAmethystIngameMenu::Quit()
{
    APlayerController* const PCOwner = PlayerOwner ? PlayerOwner->PlayerController : nullptr;
    if (PCOwner)
    {
        PCOwner->ConsoleCommand("quit");
    }
}

int32 FAmethystIngameMenu::GetOwnerUserIndex() const
{
    return PlayerOwner ? PlayerOwner->ControllerId : 0;
}


#undef LOCTEXT_NAMESPACE
