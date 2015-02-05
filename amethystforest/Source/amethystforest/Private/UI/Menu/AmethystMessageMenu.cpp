
#include "amethystforest.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Widgets/SAmethystConfirmationDialog.h"
#include "AmethystMessageMenu.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

void FAmethystMessageMenu::Construct(ULocalPlayer* InPlayerOwner, const FString& Message, const FString& OKButtonText, const FString& CancelButtonText, const FName& InPendingNextState)
{
    PlayerOwner				= InPlayerOwner;
    PendingControllerIndex	= 0;
    PendingNextState		= InPendingNextState;
    
    if (PlayerOwner)
    {
        /* TO DO: GameViewportClient
        UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(PlayerOwner->ViewportClient);
        
        if (AmethystViewport)
        {
            // Hide the previous dialog
            AmethystViewport->HideDialog();
            
            // Show the new one
            AmethystViewport->ShowDialog(
                                        PlayerOwner,
                                        Message,
                                        OKButtonText,
                                        CancelButtonText,
                                        FOnClicked::CreateRaw(this, &FAmethystMessageMenu::OnClickedOK),
                                        FOnClicked::CreateRaw(this, &FAmethystMessageMenu::OnClickedCancel)
                                        );
        }
         */
    }
}

void FAmethystMessageMenu::RemoveFromGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        /* TO DO: GameViewportClient
        UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GEngine->GameViewport);
        
        if (AmethystViewport)
        {
            // Hide the previous dialog
            AmethystViewport->HideDialog();
        }
         */
    }
}

void FAmethystMessageMenu::SetControllerAndGotoState(const int ControllerIndex, const FName& NewState)
{
    if (PlayerOwner)
    {
        UAmethystGameInstance* const GI = Cast<UAmethystGameInstance>(PlayerOwner->GetGameInstance());
        if (GI)
        {
            GI->GotoState(NewState);
        }
    }
}

FReply FAmethystMessageMenu::OnClickedOK()
{
    OKButtonDelegate.ExecuteIfBound();
    RemoveFromGameViewport();
    SetControllerAndGotoState(PendingControllerIndex, PendingNextState);
    return FReply::Handled();
}

FReply FAmethystMessageMenu::OnClickedCancel()
{
    CancelButtonDelegate.ExecuteIfBound();
    RemoveFromGameViewport();
    SetControllerAndGotoState(PendingControllerIndex, PendingNextState);
    return FReply::Handled();
}

void FAmethystMessageMenu::SetOKClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
    OKButtonDelegate = InButtonDelegate;
}

void FAmethystMessageMenu::SetCancelClickedDelegate(FMessageMenuButtonClicked InButtonDelegate)
{
    CancelButtonDelegate = InButtonDelegate;
}


#undef LOCTEXT_NAMESPACE
