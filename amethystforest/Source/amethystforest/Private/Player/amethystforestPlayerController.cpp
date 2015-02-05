

#include "amethystforest.h"
#include "Classes/Player/amethystforestPlayerController.h"
#include "Classes/Player/AmethystCharacter.h"
#include "Classes/Player/AmethystPersistentUser.h"
#include "Classes/Player/AmethystLocalPlayer.h"
#include "Classes/Weapon/AmethystWeapon.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Menu/AmethystInGameMenu.h"

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

	PlayerCameraManagerClass = AAmethystPlayerCameraManager::StaticClass();
	CheatClass = UAmethystCheatManager::StaticClass();
	
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
    /* TO DO: UpdateMenuOwner Function in IngameMenu needed
	AmethystIngameMenu->UpdateMenuOwner();
     */
}

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

	UAmethystPersistentUser* PersistentUser = GetPersistentUser();
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
	UAmethystLocalPlayer* const AmethystLP = Cast<UAmethystLocalPlayer>(Player);
	if (AmethystLP)
	{
        // TO DO: PersistentUser is a private member of UAmethystLocalPlayer, need to fix
		//return AmethystLP->PersistentUser;
	}

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



