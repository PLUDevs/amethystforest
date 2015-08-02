
#include "amethystforest.h"
#include "AmethystWelcomeMenu.h"
#include "Private/UI/Widgets/SAmethystConfirmationDialog.h"
#include "Private/UI/Style/AmethystStyle.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

class SAmethystWelcomeMenuWidget : public SCompoundWidget
{
	/** The menu that owns this widget. */
	FAmethystWelcomeMenu* MenuOwner;

	/** Animate the text so that the screen isn't static, for console cert requirements. */
	FCurveSequence TextAnimation;

	/** The actual curve that animates the text. */
	FCurveHandle TextColorCurve;

	SLATE_BEGIN_ARGS(SAmethystWelcomeMenuWidget)
	{}

	SLATE_ARGUMENT(FAmethystWelcomeMenu*, MenuOwner)

	SLATE_END_ARGS()

	virtual bool SupportsKeyboardFocus() const override
	{
		return true;
	}

	/**
	* Gets the text color based on the current state of the animation.
	*
	* @return The text color based on the current state of the animation.
	*/
	FSlateColor GetTextColor() const
	{
		return FSlateColor(FMath::Lerp(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), FLinearColor(0.5f, 0.5f, 0.5f, 1.0f), TextColorCurve.GetLerp()));
	}

	void Construct(const FArguments& InArgs)
	{
		MenuOwner = InArgs._MenuOwner;

		TextAnimation = FCurveSequence();
		const float AnimDuration = 1.5f;
		TextColorCurve = TextAnimation.AddCurve(0, AnimDuration, ECurveEaseFunction::QuadInOut);

		ChildSlot
			[
				SNew(SBorder)
				.Padding(30.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
#if PLATFORM_PS4
					.Text(LOCTEXT("PressStartPS4", "PRESS CROSS BUTTON TO PLAY"))
#else
					.Text(LOCTEXT("PressStartXboxOne", "PRESS A TO PLAY"))
#endif
					.ColorAndOpacity(this, &SAmethystWelcomeMenuWidget::GetTextColor)
					.TextStyle(FAmethystStyle::Get(), "AmethystGame.WelcomeScreen.WelcomeTextStyle")
				]
			];
	}

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		if (!TextAnimation.IsPlaying())
		{
			if (TextAnimation.IsAtEnd())
			{
				TextAnimation.PlayReverse(this->AsShared());
			}
			else
			{
				TextAnimation.Play(this->AsShared());
			}
		}
	}

	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override
	{
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		const FKey Key = InKeyEvent.GetKey();
		if (Key == EKeys::Enter)
		{
			/* TO DO: instead of user login we need to implement our own function
			MenuOwner->HandleLoginUIClosed(TSharedPtr<FUniqueNetId>(), 0);
			*/
		}
		else if (!MenuOwner->GetControlsLocked() && Key == EKeys::Gamepad_FaceButton_Bottom)
		{
			bool bSkipToMainMenu = true;

			/* TO DO: Online logic
			const auto OnlineSub = IOnlineSubsystem::Get();
			if (OnlineSub)
			{
				const auto IdentityInterface = OnlineSub->GetIdentityInterface();
				if (IdentityInterface.IsValid())
				{
					TSharedPtr<GenericApplication> GenericApplication = FSlateApplication::Get().GetPlatformApplication();
					const bool bIsLicensed = GenericApplication->ApplicationLicenseValid();

					const auto LoginStatus = IdentityInterface->GetLoginStatus(InKeyEvent.GetUserIndex());
					if (LoginStatus == ELoginStatus::NotLoggedIn || !bIsLicensed)
					{
						// Show the account picker.
						const auto ExternalUI = OnlineSub->GetExternalUIInterface();
						if (ExternalUI.IsValid())
						{
							ExternalUI->ShowLoginUI(InKeyEvent.GetUserIndex(), false, IOnlineExternalUI::FOnLoginUIClosedDelegate::CreateSP(MenuOwner, &FAmethystWelcomeMenu::HandleLoginUIClosed));
							bSkipToMainMenu = false;
						}
					}
				}
			}
			*/

			if (bSkipToMainMenu)
			{
				/* TO DO: Online logic
				const auto OnlineSub = IOnlineSubsystem::Get();
				if (OnlineSub)
				{
					const auto IdentityInterface = OnlineSub->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						TSharedPtr<FUniqueNetId> UserId = IdentityInterface->GetUniquePlayerId(InKeyEvent.GetUserIndex());
						// If we couldn't show the external login UI for any reason, or if the user is
						// already logged in, just advance to the main menu immediately.
						MenuOwner->HandleLoginUIClosed(UserId, InKeyEvent.GetUserIndex());
					}
				}
				*/
			}

			return FReply::Handled();
		}

		return FReply::Unhandled();
	}

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override
	{
		return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
	}
};

void FAmethystWelcomeMenu::Construct(TWeakObjectPtr< UAmethystGameInstance > InGameInstance)
{
	bControlsLocked = false;
	GameInstance = InGameInstance;
	PendingControllerIndex = -1;

	MenuWidget = SNew(SAmethystWelcomeMenuWidget)
		.MenuOwner(this);
}

void FAmethystWelcomeMenu::AddToGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->AddViewportWidgetContent(MenuWidget.ToSharedRef());
		FSlateApplication::Get().SetKeyboardFocus(MenuWidget);
	}
}

void FAmethystWelcomeMenu::RemoveFromGameViewport()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidget.ToSharedRef());
	}
}

void FAmethystWelcomeMenu::SetControllerAndAdvanceToMainMenu(const int ControllerIndex)
{
	if (!ensure(GameInstance.IsValid()))
	{
		return;
	}

	ULocalPlayer * NewPlayerOwner = GameInstance->GetFirstGamePlayer();

	if (NewPlayerOwner != nullptr && ControllerIndex != -1)
	{
		NewPlayerOwner->SetControllerId(ControllerIndex);
		NewPlayerOwner->SetCachedUniqueNetId(NewPlayerOwner->GetUniqueNetIdFromCachedControllerId());

		// tell gameinstance to transition to main menu
		GameInstance->GotoState(AmethystGameInstanceState::MainMenu);
	}
}

FReply FAmethystWelcomeMenu::OnContinueWithoutSavingConfirm()
{
	if (!ensure(GameInstance.IsValid()))
	{
		return FReply::Handled();
	}

	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GameInstance->GetGameViewportClient());

	if (AmethystViewport != NULL)
	{
		AmethystViewport->HideDialog();
	}
	*/
	SetControllerAndAdvanceToMainMenu(PendingControllerIndex);
	return FReply::Handled();
}

FReply FAmethystWelcomeMenu::OnConfirmGeneric()
{
	if (!ensure(GameInstance.IsValid()))
	{
		return FReply::Handled();
	}
	/* TO DO: GameViewportClient
	UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(GameInstance->GetGameViewportClient());

	if (AmethystViewport != NULL)
	{
		AmethystViewport->HideDialog();
	}
	*/
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
