

#include "amethystforest.h"
#include "../../Classes/Player/amethystforestPlayerController.h"
#include "../../Classes/Player/AmethystCharacter.h"
#include "../../Classes/Weapon/AmethystWeapon.h"
#include "../UI/Style/AmethystStyle.h"
#include "../UI/Menu/AmethystInGameMenu.h"

#define  ACH_FRAG_SOMEONE	TEXT("ACH_FRAG_SOMEONE")
#define  ACH_SOME_KILLS		TEXT("ACH_SOME_KILLS")
#define  ACH_LOTS_KILLS		TEXT("ACH_LOTS_KILLS")
#define  ACH_FINISH_MATCH	TEXT("ACH_FINISH_MATCH")
#define  ACH_LOTS_MATCHES	TEXT("ACH_LOTS_MATCHES")
#define  ACH_FIRST_WIN		TEXT("ACH_FIRST_WIN")
#define  ACH_LOTS_WIN		TEXT("ACH_LOTS_WIN")
#define  ACH_MANY_WIN		TEXT("ACH_MANY_WIN")
#define  ACH_SHOOT_BULLETS	TEXT("ACH_SHOOT_BULLETS")
#define  ACH_SHOOT_ROCKETS	TEXT("ACH_SHOOT_ROCKETS")
#define  ACH_GOOD_SCORE		TEXT("ACH_GOOD_SCORE")
#define  ACH_GREAT_SCORE	TEXT("ACH_GREAT_SCORE")
#define  ACH_PLAY_SANCTUARY	TEXT("ACH_PLAY_SANCTUARY")
#define  ACH_PLAY_HIGHRISE	TEXT("ACH_PLAY_HIGHRISE")

static const int32 SomeKillsCount = 10;
static const int32 LotsKillsCount = 20;
static const int32 LotsMatchesCount = 5;
static const int32 LotsWinsCount = 3;
static const int32 ManyWinsCount = 5;
static const int32 LotsBulletsCount = 100;
static const int32 LotsRocketsCount = 10;
static const int32 GoodScoreCount = 10;
static const int32 GreatScoreCount = 15;


AamethystforestPlayerController::AamethystforestPlayerController(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	bAllowGameActions = true;
	/* TO DO: PlayerCameraManager and CheatManager classes
	PlayerCameraManagerClass = AAmethystPlayerCameraManager::StaticClass();
	CheatClass = UAmethystCheatManager::StaticClass();
	

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnStartSessionCompleteEndItDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &AamethystforestPlayerController::OnStartSessionCompleteEndIt);
		OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &AamethystforestPlayerController::OnEndSessionComplete);
		OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &AamethystforestPlayerController::OnDestroySessionComplete);
	}
	ServerSayString = TEXT("Say");
	*/
}

void AamethystforestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI input
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AamethystforestPlayerController::OnToggleInGameMenu);

}


void AamethystforestPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Build menu only after game is initialized
	FAmethystStyle::Initialize();
	AmethystIngameMenu = MakeShareable(new FAmethystIngameMenu());
    /* TO DO: InGameMenu not constructing
	AmethystIngameMenu->Construct(this);
     */
}

void AamethystforestPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void AamethystforestPlayerController::FailedToSpawnPawn()
{
	if (StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void AamethystforestPlayerController::PawnPendingDestroy(APawn* P)
{
	FVector CameraLocation = P->GetActorLocation() + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AamethystforestPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	/* TO DO: SpectatorCamera_Implementation

	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
	*/
}

bool AamethystforestPlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(TEXT("DeathCamera"), true, GetPawn());

	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		const bool bBlocked = GetWorld()->LineTraceTest(PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

bool AamethystforestPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void AamethystforestPlayerController::ServerCheat_Implementation(const FString& Msg)
{
	if (CheatManager)
	{
		ClientMessage(ConsoleCommand(Msg));
	}
}

void AamethystforestPlayerController::SimulateInputKey(FKey Key, bool bPressed)
{
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

void AamethystforestPlayerController::OnKill()
{
	/* TO DO: Correct this logic, do we need this?
	const auto Events = Online::GetEventsInterface();
	const auto Identity = Online::GetIdentityInterface();

	if (Events.IsValid() && Identity.IsValid())
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (LocalPlayer)
		{
			int32 UserIndex = LocalPlayer->ControllerId;
			TSharedPtr<FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
			if (UniqueID.IsValid())
			{
				ACharacter* Pawn = GetCharacter();
				check(Pawn);
				FVector Location = Pawn->GetActorLocation();

				FOnlineEventParms Params;

				Params.Add(TEXT("SectionId"), FVariantData((int32)1));
				Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));
				Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0));

				Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0));
				Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)0));
				Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0));
				Params.Add(TEXT("KillTypeId"), FVariantData((int32)0));
				Params.Add(TEXT("LocationX"), FVariantData(Location.X));
				Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
				Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));
				Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0));

				Events->TriggerEvent(*UniqueID, TEXT("KillOponent"), Params);
			}
		}
	}
	*/
}

void AamethystforestPlayerController::SetPlayer(UPlayer* Player)
{
	APlayerController::SetPlayer(Player);
    /* UpdateMenuOwner Function in IngameMenu needed
	AmethystIngameMenu->UpdateMenuOwner();
     */
}

/* TO DO: Create PlayerState Class
void AamethystforestPlayerController::OnDeathMessage(class AAmethystPlayerState* KillerPlayerState, class AAmethystPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	TO DO: HUD Class
	AAmethystHUD* AmethystHUD = GetAmethystHUD();
	if (AmethystHUD)
	{
		AmethystHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);
	}

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetUniqueNetId().IsValid() && KilledPlayerState->UniqueId.IsValid())
	{
		// if this controller is the player who died, update the hero stat.
		if (*LocalPlayer->GetUniqueNetId() == *KilledPlayerState->UniqueId)
		{
			const auto Events = Online::GetEventsInterface();
			const auto Identity = Online::GetIdentityInterface();

			if (Events.IsValid() && Identity.IsValid())
			{
				int32 UserIndex = LocalPlayer->ControllerId;
				TSharedPtr<FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
				if (UniqueID.IsValid())
				{
					ACharacter* Pawn = GetCharacter();
					check(Pawn);
					FVector Location = Pawn->GetActorLocation();

					FOnlineEventParms Params;
					Params.Add(TEXT("SectionId"), FVariantData((int32)1));
					Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));
					Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0));

					Params.Add(TEXT("PlayerRoleId"), FVariantData((int32)0));
					Params.Add(TEXT("PlayerWeaponId"), FVariantData((int32)0));
					Params.Add(TEXT("EnemyRoleId"), FVariantData((int32)0));
					Params.Add(TEXT("EnemyWeaponId"), FVariantData((int32)0));

					Params.Add(TEXT("LocationX"), FVariantData(Location.X));
					Params.Add(TEXT("LocationY"), FVariantData(Location.Y));
					Params.Add(TEXT("LocationZ"), FVariantData(Location.Z));

					Events->TriggerEvent(*UniqueID, TEXT("PlayerDeath"), Params);
				}
			}
		}
	}
}
*/

void AamethystforestPlayerController::OnToggleInGameMenu()
{
	if (AmethystIngameMenu.IsValid())
	{
		AmethystIngameMenu->ToggleGameMenu();
	}
}

bool AamethystforestPlayerController::IsGameMenuVisible() const
{
	bool Result = false;
	if (AmethystIngameMenu.IsValid())
	{
		Result = AmethystIngameMenu->GetIsGameMenuUp();
	}

	return Result;
}

void AamethystforestPlayerController::SetInfiniteAmmo(bool bEnable)
{
	bInfiniteAmmo = bEnable;
}

void AamethystforestPlayerController::SetInfiniteClip(bool bEnable)
{
	bInfiniteClip = bEnable;
}

void AamethystforestPlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

void AamethystforestPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

/** Starts the online game using the session name in the PlayerState */
void AamethystforestPlayerController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	/* TO DO: PlayerState and persistentuser class and fix logic
	AAmethystPlayerState* AmethystPlayerState = Cast<AAmethystPlayerState>(PlayerState);
	if (AmethystPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *AmethystPlayerState->SessionName.ToString());
				Sessions->StartSession(AmethystPlayerState->SessionName);
			}
		}
	}
	*/
	else
	{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(FTimerDelegate::CreateUObject(this, &AamethystforestPlayerController::ClientStartOnlineGame_Implementation), 0.2f, false);
	}
}

/** Ends the online game using the session name in the PlayerState */
void AamethystforestPlayerController::ClientEndOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;
	/* TO DO: PlayerState and persistentuser class and fix logic
	AAmethystPlayerState* AmethystPlayerState = Cast<AAmethystPlayerState>(PlayerState);
	if (AmethystPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *AmethystPlayerState->SessionName.ToString());
				Sessions->EndSession(AmethystPlayerState->SessionName);
			}
		}
	}
	*/
}

void AamethystforestPlayerController::ClientReturnToMainMenu_Implementation(const FString& ReturnReason)
{
	CleanupSessionOnReturnToMenu();
}

/** Ends and/or destroys game session */
void AamethystforestPlayerController::CleanupSessionOnReturnToMenu()
{
	bool bPendingOnlineOp = false;

	// end online game and then destroy it
	/* TO DO: PlayerState and persistentuser class and fix logic
	AAmethystPlayerState* AmethystPlayerState = Cast<AAmethystPlayerState>(PlayerState);
	if (AmethystPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				EOnlineSessionState::Type SessionState = Sessions->GetSessionState(AmethystPlayerState->SessionName);
				UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *AmethystPlayerState->SessionName.ToString(), EOnlineSessionState::ToString(SessionState));

				if (EOnlineSessionState::InProgress == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *AmethystPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = Sessions->EndSession(AmethystPlayerState->SessionName);
					if (!bPendingOnlineOp)
					{
						Sessions->ClearOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					}
				}
				else if (EOnlineSessionState::Ending == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *AmethystPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = true;
				}
				else if (EOnlineSessionState::Ended == SessionState ||
					EOnlineSessionState::Pending == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *AmethystPlayerState->SessionName.ToString());
					Sessions->AddOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					bPendingOnlineOp = Sessions->DestroySession(AmethystPlayerState->SessionName);
					if (!bPendingOnlineOp)
					{
						Sessions->ClearOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					}
				}
				else if (EOnlineSessionState::Starting == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *AmethystPlayerState->SessionName.ToString());
					Sessions->AddOnStartSessionCompleteDelegate(OnStartSessionCompleteEndItDelegate);
					bPendingOnlineOp = true;
				}
			}
		}
	}
	*/

	if (!bPendingOnlineOp)
	{
		GEngine->HandleDisconnect(GetWorld(), GetWorld()->GetNetDriver());

		// Temp fix until next week's meeting on how to design King / Networking interaction properly.
		/* TO DO: Figure this out 
		UAmethystGameKing& AmethystKing = UAmethystGameKing::Get();
		AmethystKing.SetCurrentState(AmethystKing.GetInitialFrontendState());
		*/
	}
}

void AamethystforestPlayerController::OnStartSessionCompleteEndIt(FName SessionName, bool bWasSuccessful)
{
	/* TO DO: Fix this logic
	UE_LOG(LogOnline, Log, TEXT("OnStartSessionCompleteEndIt: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate(OnStartSessionCompleteEndItDelegate);
		}
	}
	*/

	// continue
	CleanupSessionOnReturnToMenu();
}

/**
* Delegate fired when ending an online session has completed
*
* @param SessionName the name of the session this callback is for
* @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void AamethystforestPlayerController::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	/* TO DO: Fix this logic
	UE_LOG(LogOnline, Log, TEXT("OnEndSessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
		}
	}
	*/

	// continue
	CleanupSessionOnReturnToMenu();
}

/**
* Delegate fired when destroying an online session has completed
*
* @param SessionName the name of the session this callback is for
* @param bWasSuccessful true if the async action completed without error, false if there was an error
*/
void AamethystforestPlayerController::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	/* TO DO: Fix this logic
	UE_LOG(LogOnline, Log, TEXT("OnDestroySessionComplete: Session=%s bWasSuccessful=%s"), *SessionName.ToString(), bWasSuccessful ? TEXT("true") : TEXT("false"));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
		}
	}
	*/

	// continue
	CleanupSessionOnReturnToMenu();
}

void AamethystforestPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
{
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);

	// If we have a pawn we need to determine if we should show/hide the weapon
	AAmethystCharacter* MyPawn = Cast<AAmethystCharacter>(GetPawn());
	AAmethystWeapon* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
	if (MyWeapon)
	{
		if (bInCinematicMode && bHidePlayer)
		{
			MyWeapon->SetActorHiddenInGame(true);
		}
		else if (!bCinematicMode)
		{
			MyWeapon->SetActorHiddenInGame(false);
		}
	}
}

void AamethystforestPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	/* TO DO: PersistentUser Class needed
	UAmethystPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
	*/
}

void AamethystforestPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AamethystforestPlayerController, bInfiniteAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AamethystforestPlayerController, bInfiniteClip, COND_OwnerOnly);
}

void AamethystforestPlayerController::Suicide()
{
	if (IsInState(NAME_Playing))
	{
		ServerSuicide();
	}
}

bool AamethystforestPlayerController::ServerSuicide_Validate()
{
	return true;
}

void AamethystforestPlayerController::ServerSuicide_Implementation()
{
	if ((GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)))
	{
		AAmethystCharacter* MyPawn = Cast<AAmethystCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

bool AamethystforestPlayerController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

bool AamethystforestPlayerController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

bool AamethystforestPlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

bool AamethystforestPlayerController::HasGodMode() const
{
	return bGodMode;
}

bool AamethystforestPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

void AamethystforestPlayerController::Say(const FString& Msg)
{
	ServerSay(Msg.Left(128));
}

bool AamethystforestPlayerController::ServerSay_Validate(const FString& Msg)
{
	return true;
}

void AamethystforestPlayerController::ServerSay_Implementation(const FString& Msg)
{
	GetWorld()->GetAuthGameMode()->Broadcast(this, Msg, ServerSayString);
}

AAmethystHUD* AamethystforestPlayerController::GetAmethystHUD() const
{
	return Cast<AAmethystHUD>(GetHUD());
}

UAmethystPersistentUser* AamethystforestPlayerController::GetPersistentUser() const
{
	/* TO DO: LocalPlayer
	UAmethystLocalPlayer* const AmethystLP = Cast<UAmethystLocalPlayer>(Player);
	if (AmethystLP)
	{
		return AmethystLP->PersistentUser;
	}
	*/

	return NULL;
}

bool AamethystforestPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
{
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

	/* TO DO: Fix this logic 
	// Update rich presence.
	const auto PresenceInterface = Online::GetPresenceInterface();
	if (PresenceInterface.IsValid() && PlayerState->UniqueId.IsValid())
	{
		FPresenceProperties Props;
		if (Result && bPause)
		{
			Props.Add(DefaultPresenceKey, FString("Paused"));
		}
		else
		{
			Props.Add(DefaultPresenceKey, FString("InGame"));
		}
		PresenceInterface->SetPresence(*PlayerState->UniqueId, Props);
	}
	*/

	return Result;
}

void AamethystforestPlayerController::ShowInGameMenu()
{
	if (AmethystIngameMenu.IsValid() && !AmethystIngameMenu->GetIsGameMenuUp())
	{
		AmethystIngameMenu->ToggleGameMenu();
	}
}



