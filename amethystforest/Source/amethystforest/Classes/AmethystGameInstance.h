
#pragma once

#include "amethystforest.h"
#include "Engine/GameInstance.h"
#include "AmethystGameInstance.generated.h"

class FVariantData;

namespace AmethystGameInstanceState
{
	extern const FName None;
	extern const FName WelcomeScreen;
	extern const FName MainMenu;
	extern const FName MessageMenu;
	extern const FName Playing;
}

/** This class holds the value of what message to display when we are in the "MessageMenu" state */
class FAmethystPendingMessage
{
public:
	FText	DisplayString;				// This is the display message in the main message body
	FText	OKButtonString;				// This is the ok button text
	FText	CancelButtonString;			// If this is not empty, it will be the cancel button text
	FName	NextState;					// Final destination state once message is discarded

	TWeakObjectPtr< ULocalPlayer > PlayerOwner;		// Owner of dialog who will have focus (can be NULL)
};

class SAmethystWaitDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAmethystWaitDialog)
	{}
	SLATE_ARGUMENT(FText, MessageText)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

private:

	/** our curve sequence and the related handles */
	FCurveSequence WidgetAnimation;

	/** used for animating the text color. */
	FCurveHandle TextColorCurve;

	/** Gets the animated text color */
	FSlateColor GetTextColor() const;
};

UCLASS(config = Game)
class UAmethystGameInstance : public UGameInstance
{
public:
	GENERATED_UCLASS_BODY()

public:

	bool Tick(float DeltaSeconds);

	class AAmethystGameSession* GetGameSession() const;

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;

	bool PlayDemo(ULocalPlayer* LocalPlayer, const FString& DemoName);

	/** Sends the game to the specified state. */
	void GotoState(FName NewState);

	/** Obtains the initial welcome state, which can be different based on platform */
	FName GetInitialState();

	/** Sends the game to the initial startup/frontend state  */
	void GotoInitialState();

	/**
	* Creates the message menu, clears other menus and sets the KingState to Message.
	*
	* @param	Message				Main message body
	* @param	OKButtonString		String to use for 'OK' button
	* @param	CancelButtonString	String to use for 'Cancel' button
	* @param	NewState			Final state to go to when message is discarded
	*/
	void ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting = true, TWeakObjectPtr< ULocalPlayer > PlayerOwner = nullptr);

	TSharedPtr< FUniqueNetId > GetUniqueNetIdFromControllerId(const int ControllerId);

	// Generic confirmation handling (just hide the dialog)
	FReply OnConfirmGeneric();
	bool HasLicense() const { return bIsLicensed; }

private:

	UPROPERTY(config)
		FString WelcomeScreenMap;

	UPROPERTY(config)
		FString MainMenuMap;


	FName CurrentState;
	FName PendingState;

	FAmethystPendingMessage PendingMessage;

	/** Whether the user has an active license to play the game */
	bool bIsLicensed;

	/** Main menu UI */
	TSharedPtr<class FAmethystMainMenu> MainMenuUI;

	/** Message menu (Shown in the even of errors - unable to connect etc) */
	TSharedPtr<class FAmethystMessageMenu> MessageMenuUI;

	/** Welcome menu UI (for consoles) */
	TSharedPtr<class FAmethystWelcomeMenu> WelcomeMenuUI;

	/** Dialog widget to show non-interactive waiting messages for network timeouts and such. */
	TSharedPtr<SAmethystWaitDialog>			WaitMessageWidget;

	/** Controller to ignore for pairing changes. -1 to skip ignore. */
	int32 IgnorePairingChangeForControllerId;

	/** Last connection status that was passed into the HandleNetworkConnectionStatusChanged hander */
	EOnlineServerConnectionStatus::Type	CurrentConnectionStatus;

	/** Delegate for callbacks to Tick */
	FTickerDelegate TickDelegate;

	/** Handle to various registered delegates */
	FDelegateHandle TickDelegateHandle;

	void OnPreLoadMap();
	void OnPostLoadMap();
	void OnPostDemoPlay();

	virtual void HandleDemoPlaybackFailure(EDemoPlayFailure::Type FailureType, const FString& ErrorString) override;

	void MaybeChangeState();
	void EndCurrentState(FName NextState);
	void BeginNewState(FName NewState, FName PrevState);

	void BeginWelcomeScreenState();
	void BeginMainMenuState();
	void BeginMessageMenuState();
	void BeginPlayingState();

	void EndWelcomeScreenState();
	void EndMainMenuState();
	void EndMessageMenuState();
	void EndPlayingState();

	void ShowLoadingScreen();

	/**
	* Creates the message menu, clears other menus and sets the KingState to Message.
	*
	* @param	Message				Main message body
	* @param	OKButtonString		String to use for 'OK' button
	* @param	CancelButtonString	String to use for 'Cancel' button
	*/
	void ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString);

	bool LoadFrontEndMap(const FString& MapName);

	// OSS delegates to handle
	void HandleUserLoginChanged(int32 GameUserIndex, ELoginStatus::Type PreviousLoginStatus, ELoginStatus::Type LoginStatus, const FUniqueNetId& UserId);

	// Callback to pause the game when the OS has constrained our app.
	void HandleAppWillDeactivate();

	// Callback occurs when game being suspended
	void HandleAppSuspend();

	// Callback occurs when game resuming
	void HandleAppResume();

	// Callback to process game licensing change notifications.
	void HandleAppLicenseUpdate();

	// Callback to handle safe frame size changes.
	void HandleSafeFrameChanged();

	// Callback to handle controller connection changes.
	void HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex);

	// Callback to handle controller pairing changes.
	FReply OnPairingUsePreviousProfile();

	// Callback to handle controller pairing changes.
	FReply OnPairingUseNewProfile();

	// Callback to handle controller pairing changes.
	void HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser);

	// Handle confirming the controller disconnected dialog.
	FReply OnControllerReconnectConfirm();

protected:
	bool HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld);
};


