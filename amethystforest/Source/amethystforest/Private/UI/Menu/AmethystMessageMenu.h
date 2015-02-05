#pragma once
#include "Slate.h"

DECLARE_DELEGATE(FMessageMenuButtonClicked);

class FAmethystMessageMenu : public TSharedFromThis<FAmethystMessageMenu>
{
public:
    /** build menu */
    void Construct(class ULocalPlayer* InPlayerOwner, const FString& Message, const FString& OKButtonText, const FString& CancelButtonText,  const FName& InPendingNextState);
    
    /** Remove from the gameviewport. */
    void RemoveFromGameViewport();
    
    /**
     * The delegate function for external login UI closure when a user has signed in.
     *
     * @param UniqueId The unique Id of the user who just signed in.
     * @param ControllerIndex The controller index of the player who just signed in.
     */
    void HandleLoginUIClosed(TSharedPtr<FUniqueNetId> UniqueId, const int ControllerIndex);
    
    /**
     * Called when a user needs to advance from the welcome screen to the main menu.
     *
     * @param ControllerIndex The controller index of the player that's advancing.
     */
    void SetControllerAndGotoState(const int ControllerIndex, const FName& NewState);
    
    void SetOKClickedDelegate(FMessageMenuButtonClicked	InButtonDelegate);
    void SetCancelClickedDelegate(FMessageMenuButtonClicked	InButtonDelegate);
    
    
private:
    
    /** Owning local player */
    ULocalPlayer* PlayerOwner;
    
    /** Cache the user id that tried to advance, so we can use it again after the confirmation dialog */
    int PendingControllerIndex;
    
    /** Cache the desired next state so we can advance to that after the confirmation dialog */
    FName PendingNextState;
    
    /** Handler for ok confirmation. */
    FReply OnClickedOK();
    FMessageMenuButtonClicked	OKButtonDelegate;
    
    FReply OnClickedCancel();
    FMessageMenuButtonClicked	CancelButtonDelegate;
};
