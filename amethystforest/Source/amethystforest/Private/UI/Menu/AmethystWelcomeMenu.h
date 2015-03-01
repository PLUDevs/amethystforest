
#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SViewport.h"
#include "Classes/AmethystGameInstance.h"

class FAmethystWelcomeMenu : public TSharedFromThis<FAmethystWelcomeMenu>
{
public:
	/** build menu */
	void Construct(TWeakObjectPtr< class UAmethystGameInstance > InGameInstance);

	/** Add the menu to the gameviewport so it becomes visible */
	void AddToGameViewport();

	/** Remove from the gameviewport. */
	void RemoveFromGameViewport();

	/**
	* Called when a user needs to advance from the welcome screen to the main menu.
	*
	* @param ControllerIndex The controller index of the player that's advancing.
	*/
	void SetControllerAndAdvanceToMainMenu(const int ControllerIndex);

	/** Lock/Unlock controls based*/
	void LockControls(bool bLock)
	{
		bControlsLocked = bLock;
	}

	bool GetControlsLocked() const
	{
		return bControlsLocked;
	}
	TWeakObjectPtr< class UAmethystGameInstance > GetGameInstance() { return GameInstance; }

private:
	/** Owning game instance */
	TWeakObjectPtr< class UAmethystGameInstance > GameInstance;

	/** Cache the user id that tried to advance, so we can use it again after the confirmation dialog */
	int PendingControllerIndex;

	/** "Presss A/X to play" widget */
	TSharedPtr<class SWidget> MenuWidget;

	/** Handler for continue-without-saving confirmation. */
	FReply OnContinueWithoutSavingConfirm();

	/** Generic Handler for hiding the dialog. */
	FReply OnConfirmGeneric();

	bool bControlsLocked;
};
