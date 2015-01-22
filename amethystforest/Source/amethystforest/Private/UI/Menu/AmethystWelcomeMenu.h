
#pragma once
#include "Slate.h"

class FAmethystWelcomeMenu : public TSharedFromThis<FAmethystWelcomeMenu>
{
public:
    /** build menu */
    void Construct(class ULocalPlayer* InPlayerOwner);
    
    /** Add the menu to the gameviewport so it becomes visible */
    void AddToGameViewport();
    
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
    void SetControllerAndAdvanceToMainMenu(const int ControllerIndex);
    
private:
    /** Owning local player */
    ULocalPlayer* PlayerOwner;
    
    /** Cache the user id that tried to advance, so we can use it again after the confirmation dialog */
    int PendingControllerIndex;
    
    /** "Presss A/X to play" widget */
    TSharedPtr<class SWidget> MenuWidget;
    
    /** Handler for continue-without-saving confirmation. */
    FReply OnContinueWithoutSavingConfirm();
    
    /** Handler for backing out of continue-without-saving. */
    FReply OnContinueWithoutSavingBack();
};
