

#include "amethystforest.h"
#include "EngineUserInterfaceClasses.h"
#include "amethystforestPlayerController.h"



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
	/* TO DO: PlayerCameraManager and CheatManager classes
	PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();
	CheatClass = UShooterCheatManager::StaticClass();
	bAllowGameActions = true;

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
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AamethystforestPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AamethystforestPlayerController::OnHideScoreboard);

	// voice chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &AamethystforestPlayerController::ToggleChatWindow);
}


void AamethystforestPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//Build menu only after game is initialized
	/* TO DO: ShooterStyle?? InGameMenu class needed 
	FShooterStyle::Initialize();
	ShooterIngameMenu = MakeShareable(new FShooterIngameMenu());
	ShooterIngameMenu->Construct(this);
	*/
}

void AamethystforestPlayerController::QueryAchievements()
{
	// precache achievements
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->ControllerId);

				if (UserId.IsValid())
				{
					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

					if (Achievements.IsValid())
					{
						Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &AamethystforestPlayerController::OnQueryAchievementsComplete));
					}
				}
				else
				{
					UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot read achievements."));
	}
}

void AamethystforestPlayerController::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	UE_LOG(LogOnline, Display, TEXT("AamethystforestPlayerController::OnQueryAchievementsComplete(bWasSuccessful = %s)"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE"));
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

void AamethystforestPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	// write stats
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
		if (ShooterPlayerState)
		{
			// update local saved profile
			UShooterPersistentUser* const PersistentUser = GetPersistentUser();
			if (PersistentUser)
			{
				/* TO DO: PlayerState Class needed
				PersistentUser->AddMatchResult(ShooterPlayerState->GetKills(), ShooterPlayerState->GetDeaths(), ShooterPlayerState->GetNumBulletsFired(), ShooterPlayerState->GetNumRocketsFired(), bIsWinner);
				PersistentUser->SaveIfDirty();
				*/
			}

			// update achievements
			UpdateAchievementsOnGameEnd();

			// update leaderboards
			IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
			if (OnlineSub)
			{
				IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
				if (Identity.IsValid())
				{
					TSharedPtr<FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->ControllerId);
					if (UserId.IsValid())
					{
						IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
						if (Leaderboards.IsValid())
						{
							FShooterAllTimeMatchResultsWrite WriteObject;

							WriteObject.SetIntStat(LEADERBOARD_STAT_SCORE, ShooterPlayerState->GetKills());
							WriteObject.SetIntStat(LEADERBOARD_STAT_KILLS, ShooterPlayerState->GetKills());
							WriteObject.SetIntStat(LEADERBOARD_STAT_DEATHS, ShooterPlayerState->GetDeaths());
							WriteObject.SetIntStat(LEADERBOARD_STAT_MATCHESPLAYED, 1);

							// the call will copy the user id and write object to its own memory
							Leaderboards->WriteLeaderboards(ShooterPlayerState->SessionName, *UserId, WriteObject);
						}
					}
				}
			}
		}
	}

	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

/* TO DO: SpectatorCamera_Implementation
void AamethystforestPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}
*/

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
}

void AamethystforestPlayerController::SetPlayer(UPlayer* Player)
{
	APlayerController::SetPlayer(Player);
	ShooterIngameMenu->UpdateMenuOwner();
}

void AamethystforestPlayerController::OnDeathMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	/* TO DO: HUD Class
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);
	}
	*/

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

void AamethystforestPlayerController::OnToggleInGameMenu()
{
	if (ShooterIngameMenu.IsValid())
	{
		ShooterIngameMenu->ToggleGameMenu();
	}
}

void AamethystforestPlayerController::OnShowScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowScoreboard(true);
	}
}

void AamethystforestPlayerController::OnHideScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	// If have a valid match and the match is over - hide the scoreboard
	if ((ShooterHUD != NULL) && (ShooterHUD->IsMatchOver() == false))
	{
		ShooterHUD->ShowScoreboard(false);
	}
}

bool AamethystforestPlayerController::IsGameMenuVisible() const
{
	bool Result = false;
	if (ShooterIngameMenu.IsValid())
	{
		Result = ShooterIngameMenu->GetIsGameMenuUp();
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

void AamethystforestPlayerController::ClientGameStarted_Implementation()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->SetMatchState(EShooterMatchState::Playing);
	}

	QueryAchievements();
}

/** Starts the online game using the session name in the PlayerState */
void AamethystforestPlayerController::ClientStartOnlineGame_Implementation()
{
	if (!IsPrimaryPlayer())
		return;

	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *ShooterPlayerState->SessionName.ToString());
				Sessions->StartSession(ShooterPlayerState->SessionName);
			}
		}
	}
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

	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *ShooterPlayerState->SessionName.ToString());
				Sessions->EndSession(ShooterPlayerState->SessionName);
			}
		}
	}
}

void AamethystforestPlayerController::ClientReturnToMainMenu_Implementation(const FString& ReturnReason)
{
	OnHideScoreboard();
	CleanupSessionOnReturnToMenu();
}

/** Ends and/or destroys game session */
void AamethystforestPlayerController::CleanupSessionOnReturnToMenu()
{
	bool bPendingOnlineOp = false;

	// end online game and then destroy it
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid())
			{
				EOnlineSessionState::Type SessionState = Sessions->GetSessionState(ShooterPlayerState->SessionName);
				UE_LOG(LogOnline, Log, TEXT("Session %s is '%s'"), *ShooterPlayerState->SessionName.ToString(), EOnlineSessionState::ToString(SessionState));

				if (EOnlineSessionState::InProgress == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Ending session %s on return to main menu"), *ShooterPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = Sessions->EndSession(ShooterPlayerState->SessionName);
					if (!bPendingOnlineOp)
					{
						Sessions->ClearOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					}
				}
				else if (EOnlineSessionState::Ending == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to end on return to main menu"), *ShooterPlayerState->SessionName.ToString());
					Sessions->AddOnEndSessionCompleteDelegate(OnEndSessionCompleteDelegate);
					bPendingOnlineOp = true;
				}
				else if (EOnlineSessionState::Ended == SessionState ||
					EOnlineSessionState::Pending == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Destroying session %s on return to main menu"), *ShooterPlayerState->SessionName.ToString());
					Sessions->AddOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					bPendingOnlineOp = Sessions->DestroySession(ShooterPlayerState->SessionName);
					if (!bPendingOnlineOp)
					{
						Sessions->ClearOnDestroySessionCompleteDelegate(OnDestroySessionCompleteDelegate);
					}
				}
				else if (EOnlineSessionState::Starting == SessionState)
				{
					UE_LOG(LogOnline, Log, TEXT("Waiting for session %s to start, and then we will end it to return to main menu"), *ShooterPlayerState->SessionName.ToString());
					Sessions->AddOnStartSessionCompleteDelegate(OnStartSessionCompleteEndItDelegate);
					bPendingOnlineOp = true;
				}
			}
		}
	}

	if (!bPendingOnlineOp)
	{
		GEngine->HandleDisconnect(GetWorld(), GetWorld()->GetNetDriver());

		// Temp fix until next week's meeting on how to design King / Networking interaction properly.
		UShooterGameKing& ShooterKing = UShooterGameKing::Get();
		ShooterKing.SetCurrentState(ShooterKing.GetInitialFrontendState());
	}
}

void AamethystforestPlayerController::OnStartSessionCompleteEndIt(FName SessionName, bool bWasSuccessful)
{
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

	// continue
	CleanupSessionOnReturnToMenu();
}

void AamethystforestPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Allow only looking around
	SetIgnoreMoveInput(true);
	bAllowGameActions = false;

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

	// Show scoreboard
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->SetMatchState(bIsWinner ? EShooterMatchState::Won : EShooterMatchState::Lost);
		if (IsPrimaryPlayer())
		{
			ShooterHUD->ShowScoreboard(true);
		}
	}
}

void AamethystforestPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
{
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);

	// If we have a pawn we need to determine if we should show/hide the weapon
	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetPawn());
	AShooterWeapon* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
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

	UShooterPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
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
		AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetPawn());
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

void AamethystforestPlayerController::ToggleChatWindow()
{
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		ShooterHUD->ToggleChat();
	}
}

void AamethystforestPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime)
{
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		if (Type == ServerSayString)
		{
			if (SenderPlayerState != PlayerState)
			{
				ShooterHUD->AddChatLine(S);
			}
		}
	}
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

AShooterHUD* AamethystforestPlayerController::GetShooterHUD() const
{
	return Cast<AShooterHUD>(GetHUD());
}


UShooterPersistentUser* AamethystforestPlayerController::GetPersistentUser() const
{
	UShooterLocalPlayer* const ShooterLP = Cast<UShooterLocalPlayer>(Player);
	if (ShooterLP)
	{
		return ShooterLP->PersistentUser;
	}

	return NULL;
}

bool AamethystforestPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
{
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

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

	return Result;
}

void AamethystforestPlayerController::ShowInGameMenu()
{
	if (ShooterIngameMenu.IsValid() && !ShooterIngameMenu->GetIsGameMenuUp())
	{
		ShooterIngameMenu->ToggleGameMenu();
	}
}
void AamethystforestPlayerController::UpdateAchievementsOnGameEnd()
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
		if (ShooterPlayerState)
		{
			const UShooterPersistentUser*  PersistentUser = GetPersistentUser();

			if (PersistentUser)
			{
				const int32 Wins = PersistentUser->GetWins();
				const int32 Losses = PersistentUser->GetLosses();
				const int32 Matches = Wins + Losses;

				const int32 TotalKills = PersistentUser->GetKills();
				const int32 MatchScore = (int32)ShooterPlayerState->GetScore();

				const int32 TotalBulletsFired = PersistentUser->GetBulletsFired();
				const int32 TotalRocketsFired = PersistentUser->GetRocketsFired();

				///////////////////////////////////////
				// Kill achievements
				{
					float fSomeKillPct = ((float)TotalKills / (float)SomeKillsCount) * 100.0f;
					fSomeKillPct = FMath::Round(fSomeKillPct);
					UpdateAchievementProgress(ACH_SOME_KILLS, fSomeKillPct);
				}

				{
					float fLotsKillPct = ((float)TotalKills / (float)LotsKillsCount) * 100.0f;
					fLotsKillPct = FMath::Round(fLotsKillPct);
					UpdateAchievementProgress(ACH_LOTS_KILLS, fLotsKillPct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Match Achievements
				{
					UpdateAchievementProgress(ACH_FINISH_MATCH, 100.0f);
				}

				{
					float fLotsRoundsPct = ((float)Matches / (float)LotsMatchesCount) * 100.0f;
					fLotsRoundsPct = FMath::Round(fLotsRoundsPct);
					UpdateAchievementProgress(ACH_LOTS_MATCHES, fLotsRoundsPct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Win Achievements
				if (Wins >= 1)
				{
					UpdateAchievementProgress(ACH_FIRST_WIN, 100.0f);
				}

				{
					float fLotsWinPct = ((float)Wins / (float)LotsWinsCount) * 100.0f;
					fLotsWinPct = FMath::Round(fLotsWinPct);
					UpdateAchievementProgress(ACH_LOTS_WIN, fLotsWinPct);
				}

				{
					float fManyWinPct = ((float)Wins / (float)ManyWinsCount) * 100.0f;
					fManyWinPct = FMath::Round(fManyWinPct);
					UpdateAchievementProgress(ACH_MANY_WIN, fManyWinPct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Ammo Achievements
				{
					float fLotsBulletsPct = ((float)TotalBulletsFired / (float)LotsBulletsCount) * 100.0f;
					fLotsBulletsPct = FMath::Round(fLotsBulletsPct);
					UpdateAchievementProgress(ACH_SHOOT_BULLETS, fLotsBulletsPct);
				}

				{
					float fLotsRocketsPct = ((float)TotalRocketsFired / (float)LotsRocketsCount) * 100.0f;
					fLotsRocketsPct = FMath::Round(fLotsRocketsPct);
					UpdateAchievementProgress(ACH_SHOOT_ROCKETS, fLotsRocketsPct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Score Achievements
				{
					float fGoodScorePct = ((float)MatchScore / (float)GoodScoreCount) * 100.0f;
					fGoodScorePct = FMath::Round(fGoodScorePct);
					UpdateAchievementProgress(ACH_GOOD_SCORE, fGoodScorePct);
				}

				{
					float fGreatScorePct = ((float)MatchScore / (float)GreatScoreCount) * 100.0f;
					fGreatScorePct = FMath::Round(fGreatScorePct);
					UpdateAchievementProgress(ACH_GREAT_SCORE, fGreatScorePct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Map Play Achievements
				UWorld* World = GetWorld();
				if (World)
				{
					FString MapName = *FPackageName::GetShortName(World->PersistentLevel->GetOutermost()->GetName());
					if (MapName.Find(TEXT("Highrise")) != -1)
					{
						UpdateAchievementProgress(ACH_PLAY_HIGHRISE, 100.0f);
					}
					else if (MapName.Find(TEXT("Sanctuary")) != -1)
					{
						UpdateAchievementProgress(ACH_PLAY_SANCTUARY, 100.0f);
					}
				}
				///////////////////////////////////////			
			}
		}
	}
}


