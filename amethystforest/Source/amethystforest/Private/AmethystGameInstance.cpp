
#include "amethystforest.h"

#include "Classes/AmethystGameInstance.h"
#include "UI/Style/AmethystStyle.h"
#include "UI/Style/AmethystMenuItemWidgetStyle.h"
#include "UI/Menu/AmethystMainMenu.h"
#include "UI/Menu/AmethystWelcomeMenu.h"
#include "UI/Menu/AmethystMessageMenu.h"
#include "Classes/Game/AmethystGameState.h"
#include "Classes/Player/amethystforestPlayerController.h"
#include "Classes/Player/AmethystPlayerController_Menu.h"
#include "AmethystGameViewportClient.h"
//#include "AmethystGameLoadingScreen.h"

void SAmethystWaitDialog::Construct(const FArguments& InArgs)
{
	const FAmethystMenuItemStyle* ItemStyle = &FAmethystStyle::Get().GetWidgetStyle<FAmethystMenuItemStyle>("DefaultAmethystMenuItemStyle");
	const FButtonStyle* ButtonStyle = &FAmethystStyle::Get().GetWidgetStyle<FButtonStyle>("DefaultAmethystButtonStyle");
	ChildSlot
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(20.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.Padding(50.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.BorderImage(&ItemStyle->BackgroundBrush)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
				[
					SNew(STextBlock)
					.TextStyle(FAmethystStyle::Get(), "AmethystGame.MenuHeaderTextStyle")
					.ColorAndOpacity(this, &SAmethystWaitDialog::GetTextColor)
					.Text(InArgs._MessageText)
					.WrapTextAt(500.0f)
				]
			]
		];

	//Setup a curve
	const float StartDelay = 0.0f;
	const float SecondDelay = 0.0f;
	const float AnimDuration = 2.0f;

	WidgetAnimation = FCurveSequence();
	TextColorCurve = WidgetAnimation.AddCurve(StartDelay + SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	WidgetAnimation.Play();
}

FSlateColor SAmethystWaitDialog::GetTextColor() const
{
	//instead of going from black -> white, go from white -> grey.
	float fAlpha = 1.0f - TextColorCurve.GetLerpLooping();
	fAlpha = fAlpha * 0.5f + 0.5f;
	return FLinearColor(FColor(155, 164, 182, FMath::Clamp((int32)(fAlpha * 255.0f), 0, 255)));
}

namespace AmethystGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName WelcomeScreen = FName(TEXT("WelcomeScreen"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName MessageMenu = FName(TEXT("MessageMenu"));
	const FName Playing = FName(TEXT("Playing"));
}


UAmethystGameInstance::UAmethystGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsLicensed(true) // Default to licensed (should have been checked by OS on boot)
{
	CurrentState = AmethystGameInstanceState::None;
}

void UAmethystGameInstance::Init()
{
	Super::Init();

	IgnorePairingChangeForControllerId = -1;

	FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this, &UAmethystGameInstance::HandleAppWillDeactivate);

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this, &UAmethystGameInstance::HandleAppSuspend);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UAmethystGameInstance::HandleAppResume);

	FCoreDelegates::OnSafeFrameChangedEvent.AddUObject(this, &UAmethystGameInstance::HandleSafeFrameChanged);
	FCoreDelegates::OnControllerConnectionChange.AddUObject(this, &UAmethystGameInstance::HandleControllerConnectionChange);
	FCoreDelegates::ApplicationLicenseChange.AddUObject(this, &UAmethystGameInstance::HandleAppLicenseUpdate);

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAmethystGameInstance::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UAmethystGameInstance::OnPostLoadMap);

	FCoreUObjectDelegates::PostDemoPlay.AddUObject(this, &UAmethystGameInstance::OnPostDemoPlay);

	// Register delegate for ticker callback
	TickDelegate = FTickerDelegate::CreateUObject(this, &UAmethystGameInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);
}

void UAmethystGameInstance::Shutdown()
{
	Super::Shutdown();

	// Unregister ticker delegate
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

void UAmethystGameInstance::OnPreLoadMap()
{

}

void UAmethystGameInstance::OnPostLoadMap()
{
	// Make sure we hide the loading screen when the level is done loading
	/* TO DO: GameViewport
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

	if (AmethystViewport != NULL)
	{
		AmethystViewport->HideLoadingScreen();
	}
	*/
}

void UAmethystGameInstance::OnPostDemoPlay()
{
	GotoState(AmethystGameInstanceState::Playing);
}

void UAmethystGameInstance::HandleDemoPlaybackFailure(EDemoPlayFailure::Type FailureType, const FString& ErrorString)
{
	ShowMessageThenGotoState(FText::Format(NSLOCTEXT("UAmethystGameInstance", "DemoPlaybackFailedFmt", "Demo playback failed: {0}"), FText::FromString(ErrorString)), NSLOCTEXT("DialogButtons", "OKAY", "OK"), FText::GetEmpty(), AmethystGameInstanceState::MainMenu);
}

void UAmethystGameInstance::StartGameInstance()
{
#if PLATFORM_PS4 == 0
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
#endif

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

void UAmethystGameInstance::ShowMessageThenGotoState(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString, const FName& NewState, const bool OverrideExisting, TWeakObjectPtr< ULocalPlayer > PlayerOwner)
{
	/* TO DO: Add logs to each stage */
	const bool bAtWelcomeScreen = PendingState == AmethystGameInstanceState::WelcomeScreen || CurrentState == AmethystGameInstanceState::WelcomeScreen;

	// Never override the welcome screen
	if (bAtWelcomeScreen)
	{
		return;
	}

	const bool bAlreadyAtMessageMenu = PendingState == AmethystGameInstanceState::MessageMenu || CurrentState == AmethystGameInstanceState::MessageMenu;
	const bool bAlreadyAtDestState = PendingState == NewState || CurrentState == NewState;

	// If we are already going to the message menu, don't override unless asked to
	if (bAlreadyAtMessageMenu && PendingMessage.NextState == NewState && !OverrideExisting)
	{
		return;
	}

	// If we are already going to the message menu, and the next dest is welcome screen, don't override
	if (bAlreadyAtMessageMenu && PendingMessage.NextState == AmethystGameInstanceState::WelcomeScreen)
	{
		return;
	}

	// If we are already at the dest state, don't override unless asked
	if (bAlreadyAtDestState && !OverrideExisting)
	{
		return;
	}

	PendingMessage.DisplayString = Message;
	PendingMessage.OKButtonString = OKButtonString;
	PendingMessage.CancelButtonString = CancelButtonString;
	PendingMessage.NextState = NewState;
	PendingMessage.PlayerOwner = PlayerOwner;

	if (CurrentState == AmethystGameInstanceState::MessageMenu)
	{
		EndMessageMenuState();
		BeginMessageMenuState();
	}
	else
	{
		GotoState(AmethystGameInstanceState::MessageMenu);
	}
}

void UAmethystGameInstance::ShowLoadingScreen()
{
	/* TO DO: Loading Screen Needed and GameViewport!
	// This can be confusing, so here is what is happening:
	//	For LoadMap, we use the IAmethystGameLoadingScreenModule interface to show the load screen
	//  This is necessary since this is a blocking call, and our viewport loading screen won't get updated.
	//  We can't use IAmethystGameLoadingScreenModule for seamless travel though
	//  In this case, we just add a widget to the viewport, and have it update on the main thread
	//  To simplify things, we just do both, and you can't tell, one will cover the other if they both show at the same time
	IAmethystGameLoadingScreenModule* const LoadingScreenModule = FModuleManager::LoadModulePtr<IAmethystGameLoadingScreenModule>("AmethystGameLoadingScreen");
	if (LoadingScreenModule != nullptr)
	{
		LoadingScreenModule->StartInGameLoadingScreen();
	}

	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

	if (AmethystViewport != NULL)
	{
		AmethystViewport->ShowLoadingScreen();
	}
	*/
}

bool UAmethystGameInstance::LoadFrontEndMap(const FString& MapName)
{
	bool bSuccess = true;

	// if already loaded, do nothing
	UWorld* const World = GetWorld();
	if (World)
	{
		FString const CurrentMapName = *World->PersistentLevel->GetOutermost()->GetName();
		//if (MapName.Find(TEXT("Highrise")) != -1)
		if (CurrentMapName == MapName)
		{
			return bSuccess;
		}
	}

	FString Error;
	EBrowseReturnVal::Type BrowseRet = EBrowseReturnVal::Failure;
	FURL URL(
		*FString::Printf(TEXT("%s"), *MapName)
		);

	if (URL.Valid && !HasAnyFlags(RF_ClassDefaultObject)) //CastChecked<UEngine>() will fail if using Default__AmethystGameInstance, so make sure that we're not default
	{
		BrowseRet = GetEngine()->Browse(*WorldContext, URL, Error);

		// Handle failure.
		if (BrowseRet != EBrowseReturnVal::Success)
		{
			UE_LOG(LogLoad, Fatal, TEXT("%s"), *FString::Printf(TEXT("Failed to enter %s: %s. Please check the log for errors."), *MapName, *Error));
			bSuccess = false;
		}
	}
	return bSuccess;
}

AAmethystGameSession* UAmethystGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameMode* const Game = World->GetAuthGameMode();
		if (Game)
		{
			/* TO DO: Fix this
			return Cast<AAmethystGameSession>(Game->GameSession);
			*/
		}
	}

	return nullptr;
}

void UAmethystGameInstance::ShowMessageThenGoMain(const FText& Message, const FText& OKButtonString, const FText& CancelButtonString)
{
	ShowMessageThenGotoState(Message, OKButtonString, CancelButtonString, AmethystGameInstanceState::MainMenu);
}

void UAmethystGameInstance::GotoState(FName NewState)
{
	PendingState = NewState;
}

void UAmethystGameInstance::MaybeChangeState()
{
	if ((PendingState != CurrentState) && (PendingState != AmethystGameInstanceState::None))
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
	LocalPlayer->SetCachedUniqueNetId(nullptr);
	check(!WelcomeMenuUI.IsValid());
	WelcomeMenuUI = MakeShareable(new FAmethystWelcomeMenu);
	WelcomeMenuUI->Construct(this);
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
	// Make sure we're not showing the loadscreen
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

	if (AmethystViewport != NULL)
	{
		AmethystViewport->HideLoadingScreen();
	}

	// Set presence to menu state for the owning player
	SetPresenceForLocalPlayers(FVariantData(FString(TEXT("OnMenu"))));

	// load startup map
	LoadFrontEndMap(MainMenuMap);

	// player 0 gets to own the UI
	ULocalPlayer* const Player = GetFirstGamePlayer();

	MainMenuUI = MakeShareable(new FAmethystMainMenu());
	MainMenuUI->Construct(this, Player);
	MainMenuUI->AddMenuToGameViewport();
	*/
	ULocalPlayer* const Player = GetFirstGamePlayer();
#if !Amethyst_CONSOLE_UI
	// The cached unique net ID is usually set on the welcome screen, but there isn't
	// one on PC/Mac, so do it here.
	if (Player != nullptr)
	{
		Player->SetControllerId(0);
		Player->SetCachedUniqueNetId(Player->GetUniqueNetIdFromCachedControllerId());
	}
#endif

	
}

void UAmethystGameInstance::EndMainMenuState()
{
	if (MainMenuUI.IsValid())
	{
		MainMenuUI->RemoveMenuFromGameViewport();
		MainMenuUI = nullptr;
	}
}

void UAmethystGameInstance::BeginMessageMenuState()
{
	if (PendingMessage.DisplayString.IsEmpty())
	{
		GotoInitialState();
		return;
	}

	// Make sure we're not showing the loadscreen
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());
	

	if (AmethystViewport != NULL)
	{
		AmethystViewport->HideLoadingScreen();
	}
	*/

	check(!MessageMenuUI.IsValid());
	MessageMenuUI = MakeShareable(new FAmethystMessageMenu);
	/* TO DO: Should we construct this? 
	MessageMenuUI->Construct(this, PendingMessage.PlayerOwner, PendingMessage.DisplayString, PendingMessage.OKButtonString, PendingMessage.CancelButtonString, PendingMessage.NextState);
	*/

	PendingMessage.DisplayString = FText::GetEmpty();
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
	// Make sure viewport has focus
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void UAmethystGameInstance::EndPlayingState()
{
	UWorld* const World = GetWorld();
	AAmethystGameState* const GameState = World != NULL ? World->GetGameState<AAmethystGameState>() : NULL;

	if (GameState)
	{
		// Send round end events for local players
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			auto AmethystPC = Cast<AamethystforestPlayerController>(LocalPlayers[i]->PlayerController);
			if (AmethystPC)
			{
				// Assuming you can't win if you quit early
				/* TO DO: added this function in Gamestate
				AmethystPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
				*/
			}
		}

		// Give the game state a chance to cleanup first
		GameState->RequestFinishAndExitToMainMenu();
	}
}

bool UAmethystGameInstance::PlayDemo(ULocalPlayer* LocalPlayer, const FString& DemoName)
{
	ShowLoadingScreen();

	// Play the demo
	GEngine->Exec(GetWorld(), *FString::Printf(TEXT("DEMOPLAY %s"), *DemoName));

	return true;
}

bool UAmethystGameInstance::Tick(float DeltaSeconds)
{
	// Dedicated server doesn't need to worry about game state
	if (IsRunningDedicatedServer() == true)
	{
		return true;
	}

	MaybeChangeState();

	
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

	if (CurrentState != AmethystGameInstanceState::WelcomeScreen)
	{
		// If at any point we aren't licensed (but we are after welcome screen) bounce them back to the welcome screen
		if (!bIsLicensed && CurrentState != AmethystGameInstanceState::None && !AmethystViewport->IsShowingDialog())
		{
			const FText ReturnReason = NSLOCTEXT("ProfileMessages", "NeedLicense", "The signed in users do not have a license for this game. Please purchase AmethystGame from the Xbox Marketplace or sign in a user with a valid license.");
			const FText OKButton = NSLOCTEXT("DialogButtons", "OKAY", "OK");

			ShowMessageThenGotoState(ReturnReason, OKButton, FText::GetEmpty(), AmethystGameInstanceState::WelcomeScreen);
		}

		// Show controller disconnected dialog if any local players have an invalid controller
		if (AmethystViewport != NULL &&
			!AmethystViewport->IsShowingDialog())
		{
			for (int i = 0; i < LocalPlayers.Num(); ++i)
			{
				if (LocalPlayers[i] && LocalPlayers[i]->GetControllerId() == -1)
				{
					AmethystViewport->ShowDialog(
						LocalPlayers[i],
						EAmethystDialogType::ControllerDisconnected,
						FText::Format(NSLOCTEXT("ProfileMessages", "PlayerReconnectControllerFmt", "Player {0}, please reconnect your controller."), FText::AsNumber(i + 1)),
#ifdef PLATFORM_PS4
						NSLOCTEXT("DialogButtons", "PS4_CrossButtonContinue", "Cross Button - Continue"),
#else
						NSLOCTEXT("DialogButtons", "AButtonContinue", "A - Continue"),
#endif
						FText::GetEmpty(),
						FOnClicked::CreateUObject(this, &UAmethystGameInstance::OnControllerReconnectConfirm),
						FOnClicked()
						);
				}
			}
		}
	}
	
	return true;
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

void UAmethystGameInstance::HandleAppWillDeactivate()
{
	if (CurrentState == AmethystGameInstanceState::Playing)
	{
		// Just have the first player controller pause the game.
		UWorld* const GameWorld = GetWorld();
		if (GameWorld)
		{
			// protect against a second pause menu loading on top of an existing one if someone presses the Jewel / PS buttons.
			bool bNeedsPause = true;
			for (FConstControllerIterator It = GameWorld->GetControllerIterator(); It; ++It)
			{
				AamethystforestPlayerController* Controller = Cast<AamethystforestPlayerController>(*It);
				if (Controller && (Controller->IsPaused() || Controller->IsGameMenuVisible()))
				{
					bNeedsPause = false;
					break;
				}
			}

			if (bNeedsPause)
			{
				AamethystforestPlayerController* const Controller = Cast<AamethystforestPlayerController>(GameWorld->GetFirstPlayerController());
				if (Controller) 
				{
					Controller->ShowInGameMenu();
				}
			}
		}
	}
}

void UAmethystGameInstance::HandleAppSuspend()
{
	// Players will lose connection on resume. However it is possible the game will exit before we get a resume, so we must kick off round end events here.
	UWorld* const World = GetWorld();
	AAmethystGameState* const GameState = World != NULL ? World->GetGameState<AAmethystGameState>() : NULL;

	if (CurrentState != AmethystGameInstanceState::None && CurrentState != GetInitialState())
	{

		// Send round end events for local players
		for (int i = 0; i < LocalPlayers.Num(); ++i)
		{
			auto AmethystPC = Cast<AamethystforestPlayerController>(LocalPlayers[i]->PlayerController);
			if (AmethystPC)
			{
				// Assuming you can't win if you quit early
				/* TO DO: Add function in player controller 
				AmethystPC->ClientSendRoundEndEvent(false, GameState->ElapsedTime);
				*/
			}
		}
	}
}

void UAmethystGameInstance::HandleAppResume()
{
	/* TO DO: Hmmmm */
}

void UAmethystGameInstance::HandleAppLicenseUpdate()
{
	TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
	bIsLicensed = GenericApplication->ApplicationLicenseValid();
}

void UAmethystGameInstance::HandleSafeFrameChanged()
{
	UCanvas::UpdateAllCanvasSafeZoneData();
}

FReply UAmethystGameInstance::OnPairingUsePreviousProfile()
{
	// Do nothing (except hide the message) if they want to continue using previous profile
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

	if (AmethystViewport != nullptr)
	{
		AmethystViewport->HideDialog();
	}
	*/

	return FReply::Handled();
}

FReply UAmethystGameInstance::OnPairingUseNewProfile()
{
	return FReply::Handled();
}

void UAmethystGameInstance::HandleControllerPairingChanged(int GameUserIndex, const FUniqueNetId& PreviousUser, const FUniqueNetId& NewUser)
{
	if (CurrentState == AmethystGameInstanceState::WelcomeScreen)
	{
		// Don't care about pairing changes at welcome screen
		return;
	}

#if Amethyst_CONSOLE_UI && PLATFORM_XBOXONE
	if (IgnorePairingChangeForControllerId != -1 && GameUserIndex == IgnorePairingChangeForControllerId)
	{
		// We were told to ignores
		IgnorePairingChangeForControllerId = -1;	// Reset now so there there is no chance this remains in a bad state
		return;
	}

	if (PreviousUser.IsValid() && !NewUser.IsValid())
	{
		// Treat this as a disconnect or signout, which is handled somewhere else
		return;
	}

	if (!PreviousUser.IsValid() && NewUser.IsValid())
	{
		// Treat this as a signin
		ULocalPlayer * ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

		if (ControlledLocalPlayer != NULL && !ControlledLocalPlayer->GetCachedUniqueNetId().IsValid())
		{
			// If a player that previously selected "continue without saving" signs into this controller, move them back to welcome screen
			HandleSignInChangeMessaging();
		}

		return;
	}

	// Find the local player currently being controlled by this controller
	ULocalPlayer * ControlledLocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

	// See if the newly assigned profile is in our local player list
	ULocalPlayer * NewLocalPlayer = FindLocalPlayerFromUniqueNetId(NewUser);

	// If the local player being controlled is not the target of the pairing change, then give them a chance 
	// to continue controlling the old player with this controller
	if (ControlledLocalPlayer != nullptr && ControlledLocalPlayer != NewLocalPlayer)
	{
		UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());

		if (AmethystViewport != nullptr)
		{
			AmethystViewport->ShowDialog(
				nullptr,
				EAmethystDialogType::Generic,
				NSLOCTEXT("ProfileMessages", "PairingChanged", "Your controller has been paired to another profile, would you like to switch to this new profile now? Selecting YES will sign out of the previous profile."),
				NSLOCTEXT("DialogButtons", "YES", "A - YES"),
				NSLOCTEXT("DialogButtons", "NO", "B - NO"),
				FOnClicked::CreateUObject(this, &UAmethystGameInstance::OnPairingUseNewProfile),
				FOnClicked::CreateUObject(this, &UAmethystGameInstance::OnPairingUsePreviousProfile)
				);
		}
	}
#endif
}

void UAmethystGameInstance::HandleControllerConnectionChange(bool bIsConnection, int32 Unused, int32 GameUserIndex)
{
	if (!bIsConnection)
	{
		// Controller was disconnected

		// Find the local player associated with this user index
		ULocalPlayer * LocalPlayer = FindLocalPlayerFromControllerId(GameUserIndex);

		if (LocalPlayer == NULL)
		{
			return;		// We don't care about players we aren't tracking
		}

		// Invalidate this local player's controller id.
		LocalPlayer->SetControllerId(-1);
	}
}

FReply UAmethystGameInstance::OnControllerReconnectConfirm()
{
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());
	if (AmethystViewport)
	{
		AmethystViewport->HideDialog();
	}
	*/
	return FReply::Handled();
}

FReply UAmethystGameInstance::OnConfirmGeneric()
{
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GetGameViewportClient());
	if (AmethystViewport)
	{
		AmethystViewport->HideDialog();
	}
	*/

	return FReply::Handled();
}
