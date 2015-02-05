
#include "amethystforest.h"

#include "Classes/AmethystGameInstance.h"
#include "UI/Menu/AmethystMainMenu.h"
#include "UI/Menu/AmethystWelcomeMenu.h"
#include "UI/Menu/AmethystMessageMenu.h"
//#include "AmethystGameLoadingScreen.h"

namespace AmethystGameInstanceState
{
    const FName None = FName(TEXT("None"));
    const FName WelcomeScreen = FName(TEXT("WelcomeScreen"));
    const FName MainMenu = FName(TEXT("MainMenu"));
    const FName MessageMenu = FName(TEXT("MessageMenu"));
    const FName Playing = FName(TEXT("Playing"));
}


UAmethystGameInstance::UAmethystGameInstance(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
    CurrentState = AmethystGameInstanceState::None;
}

void UAmethystGameInstance::Init()
{
    Super::Init();
    
    FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &UAmethystGameInstance::HandleSafeFrameChanged);
}

void UAmethystGameInstance::StartGameInstance()
{
    TCHAR Parm[4096] = TEXT("");
    
    const TCHAR* Cmd = FCommandLine::Get();
    
    // Catch the case where we want to override the map name on startup (used for connecting to other MP instances)
    if (FParse::Token(Cmd, Parm, ARRAY_COUNT(Parm), 0) && Parm[0] != '-')
    {
        // if we're 'overriding' with the default map anyway, don't set a bogus 'playing' state.
        if (!MainMenuMap.Contains(Parm))
        {
            FURL DefaultURL;
            DefaultURL.LoadURLConfig(TEXT("DefaultPlayer"), GGameIni);
            
            FURL URL(&DefaultURL, Parm, TRAVEL_Partial);
            
            if (URL.Valid)
            {
                UEngine* const Engine = GetEngine();
                
                FString Error;
                
                const EBrowseReturnVal::Type BrowseRet = Engine->Browse(*WorldContext, URL, Error);
                
                if (BrowseRet == EBrowseReturnVal::Success)
                {
                    // Success, we loaded the map, go directly to playing state
                    GotoState(AmethystGameInstanceState::Playing);
                    return;
                }
                else if (BrowseRet == EBrowseReturnVal::Pending)
                {
                    // Assume network connection
                    LoadFrontEndMap(MainMenuMap);
                    ShowLoadingScreen();
                    GotoState(AmethystGameInstanceState::Playing);
                    return;
                }
            }
        }
    }
    
    GotoInitialState();
}

FName UAmethystGameInstance::GetInitialState()
{
#if Amethyst_CONSOLE_UI
    // Start in the welcome screen state on consoles
    return AmethystGameInstanceState::WelcomeScreen;
#else
    // On PC, go directly to the main menu
    return AmethystGameInstanceState::MainMenu;
#endif
}

void UAmethystGameInstance::GotoInitialState()
{
    GotoState(GetInitialState());
}

void UAmethystGameInstance::ShowMessageThenGotoState( const FString& Message, const FString& OKButtonString, const FString& CancelButtonString, const FName& NewState )
{
    PendingMessage.DisplayString		= Message;
    PendingMessage.OKButtonString		= OKButtonString;
    PendingMessage.CancelButtonString	= CancelButtonString;
    PendingMessage.NextState			= NewState;
    
    GotoState(AmethystGameInstanceState::MessageMenu);
}

void UAmethystGameInstance::ShowLoadingScreen()
{
    /* TO DO: LoadingScreen
    IAmethystGameLoadingScreenModule* const LoadingScreenModule = FModuleManager::LoadModulePtr<IAmethystGameLoadingScreenModule>("AmethystGameLoadingScreen");
    if (LoadingScreenModule != nullptr)
    {
        LoadingScreenModule->StartInGameLoadingScreen();
    }
     */
}

void UAmethystGameInstance::LoadFrontEndMap(const FString& MapName)
{
    // if already loaded, do nothing
    UWorld* const World = GetWorld();
    if (World)
    {
        FString const CurrentMapName = *World->PersistentLevel->GetOutermost()->GetName();
        //if (MapName.Find(TEXT("Highrise")) != -1)
        if (CurrentMapName == MapName)
        {
            return;
        }
    }
    
    FString Error;
    EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
    FURL URL(
             *FString::Printf(TEXT("%s"), *MapName)
             );
    if (URL.Valid)
    {
        BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);
    }
    
    // Handle failure.
    if (BrowseRet != EBrowseReturnVal::Success)
    {
        UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *MapName, *Error));
    }
}

void UAmethystGameInstance::GotoState(FName NewState)
{
    PendingState = NewState;
}

void UAmethystGameInstance::MaybeChangeState()
{
    if ( (PendingState != CurrentState) && (PendingState != AmethystGameInstanceState::None) )
    {
        FName const OldState = CurrentState;
        
        // end current state
        EndCurrentState(PendingState);
        
        // begin new state
        BeginNewState(PendingState, OldState);
        
        // clear pending change
        PendingState = AmethystGameInstanceState::None;
    }
}

void UAmethystGameInstance::EndCurrentState(FName NextState)
{
    // per-state custom ending code here
    if (CurrentState == AmethystGameInstanceState::WelcomeScreen)
    {
        EndWelcomeScreenState();
    }
    else if (CurrentState == AmethystGameInstanceState::MainMenu)
    {
        EndMainMenuState();
    }
    else if (CurrentState == AmethystGameInstanceState::MessageMenu)
    {
        EndMessageMenuState();
    }
    else if (CurrentState == AmethystGameInstanceState::Playing)
    {
        EndPlayingState();
    }
    
    CurrentState = AmethystGameInstanceState::None;
}

void UAmethystGameInstance::BeginNewState(FName NewState, FName PrevState)
{
    // per-state custom starting code here
    
    if (NewState == AmethystGameInstanceState::WelcomeScreen)
    {
        BeginWelcomeScreenState();
    }
    else if (NewState == AmethystGameInstanceState::MainMenu)
    {
        BeginMainMenuState();
    }
    else if (NewState == AmethystGameInstanceState::MessageMenu)
    {
        BeginMessageMenuState();
    }
    else if (NewState == AmethystGameInstanceState::Playing)
    {
        BeginPlayingState();
    }
    
    CurrentState = NewState;
}


void UAmethystGameInstance::BeginWelcomeScreenState()
{
    LoadFrontEndMap(WelcomeScreenMap);
    
    ULocalPlayer* const LocalPlayer = GetFirstGamePlayer();
    
    check(!WelcomeMenuUI.IsValid());
    WelcomeMenuUI = MakeShareable(new FAmethystWelcomeMenu);
    WelcomeMenuUI->Construct(LocalPlayer);
    WelcomeMenuUI->AddToGameViewport();
}

void UAmethystGameInstance::EndWelcomeScreenState()
{
    if (WelcomeMenuUI.IsValid())
    {
        WelcomeMenuUI->RemoveFromGameViewport();
        WelcomeMenuUI = nullptr;
    }
}

void UAmethystGameInstance::BeginMainMenuState()
{
    // load startup map
    LoadFrontEndMap(MainMenuMap);
    
    // player 0 gets to own the UI
    ULocalPlayer* const Player = GetFirstGamePlayer();
    
    MainMenuUI = MakeShareable(new FAmethystMainMenu());
    MainMenuUI->Construct(Player);
    MainMenuUI->AddMenuToGameViewport();
    
    // So we don't see two screens while in this menu
    // Since we now use GamePlayers to represent splitscreen, if we don't override this, we will see two screens in the menu when someone joins
    GetGameViewportClient()->SetDisableSplitscreenOverride( true );
}
void UAmethystGameInstance::EndMainMenuState()
{
    if (MainMenuUI.IsValid())
    {
        MainMenuUI->RemoveMenuFromGameViewport();
        MainMenuUI = nullptr;
    }
    
    // Re-enable splitscreen
    GetGameViewportClient()->SetDisableSplitscreenOverride( false );
}

void UAmethystGameInstance::BeginMessageMenuState()
{
    if (PendingMessage.DisplayString.IsEmpty())
    {
        GotoInitialState();
        return;
    }
    
    // player 0 gets to own the UI
    ULocalPlayer* const Player = GetFirstGamePlayer();
    
    check(!MessageMenuUI.IsValid());
    MessageMenuUI = MakeShareable(new FAmethystMessageMenu);
    MessageMenuUI->Construct(Player, PendingMessage.DisplayString, PendingMessage.OKButtonString, PendingMessage.CancelButtonString, PendingMessage.NextState);
    
    PendingMessage.DisplayString.Empty();
}

void UAmethystGameInstance::EndMessageMenuState()
{
    if (MessageMenuUI.IsValid())
    {
        MessageMenuUI->RemoveFromGameViewport();
        MessageMenuUI = nullptr;
    }
}

void UAmethystGameInstance::BeginPlayingState()
{
}

void UAmethystGameInstance::EndPlayingState()
{
    UWorld* const World = GetWorld();
    if (World)
    {
        AAmethystGameState* const GameState = World->GetGameState<AAmethystGameState>();
        if (GameState)
        {
            GameState->RequestFinishAndExitToMainMenu();
        }
    }
}

bool UAmethystGameInstance::HandleOpenCommand(const TCHAR* Cmd, FOutputDevice& Ar, UWorld* InWorld)
{
    bool const bOpenSuccessful = Super::HandleOpenCommand(Cmd, Ar, InWorld);
    if (bOpenSuccessful)
    {
        GotoState(AmethystGameInstanceState::Playing);
    }
    
    return bOpenSuccessful;
}

bool UAmethystGameInstance::Tick(float DeltaSeconds)
{
    // Dedicated server doesn't need to worry about game state
    if (IsRunningDedicatedServer() == true)
    {
        return true;
    }
    
    MaybeChangeState();
    return true;
}

void UAmethystGameInstance::HandleSafeFrameChanged()
{
    UCanvas::UpdateAllCanvasSafeZoneData();
}
