
#pragma once

#include "amethystforest.h"
#include "Engine/GameInstance.h"
#include "AmethystGameInstance.generated.h"



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
    FString	DisplayString;				// This is the display message in the main message body
    FString	OKButtonString;				// This is the ok button text
    FString CancelButtonString;			// If this is not empty, it will be the cancel button text
    FName	NextState;					// Final destination state once message is discarded
};

UCLASS(config=Game)
class UAmethystGameInstance : public UGameInstance, public FTickerObjectBase
{
public:
    GENERATED_UCLASS_BODY()
    
public:
    
    
    //FTicker Funcs
    virtual bool Tick(float DeltaSeconds) override;
    
    virtual void Init() override;
    virtual void StartGameInstance() override;
    
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
    void ShowMessageThenGotoState( const FString& Message, const FString& OKButtonString, const FString& CancelButtonString, const FName& NewState );
    
private:
    
    UPROPERTY(config)
    FString WelcomeScreenMap;
    
    UPROPERTY(config)
    FString MainMenuMap;
    
    
    FName CurrentState;
    FName PendingState;
    FAmethystPendingMessage PendingMessage;
    
    /** Main menu UI */
    TSharedPtr<class FAmethystMainMenu> MainMenuUI;
    
    /** Message menu (Shown in the even of errors - unable to connect etc) */
    TSharedPtr<class FAmethystMessageMenu> MessageMenuUI;
    
    /** Welcome menu UI (for consoles) */
    TSharedPtr<class FAmethystWelcomeMenu> WelcomeMenuUI;
    
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
    
    void LoadFrontEndMap(const FString& MapName);
    
    // Callback to handle safe frame size changes.
    void HandleSafeFrameChanged();
    
protected:
    bool HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld);
};


