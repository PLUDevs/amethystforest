
#include "amethystforest.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "SAmethystConfirmationDialog.h"
#include "Private/UI/Style/AmethystMenuItemWidgetStyle.h"

void SAmethystConfirmationDialog::Construct( const FArguments& InArgs )
{
    PlayerOwner = InArgs._PlayerOwner;
    
    OnConfirm = InArgs._OnConfirmClicked;
    OnCancel = InArgs._OnCancelClicked;
    
    const FAmethystMenuItemStyle* ItemStyle = &FAmethystStyle::Get().GetWidgetStyle<FAmethystMenuItemStyle>("DefaultAmethystMenuItemStyle");
    const FButtonStyle* ButtonStyle = &FAmethystStyle::Get().GetWidgetStyle<FButtonStyle>("DefaultAmethystButtonStyle");
    FLinearColor MenuTitleTextColor =  FLinearColor(FColor(155,164,182));
    ChildSlot
    [
     SNew( SVerticalBox )
     +SVerticalBox::Slot()
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
       SNew( STextBlock )
       .TextStyle(FAmethystStyle::Get(), "AmethystGame.MenuHeaderTextStyle")
       .ColorAndOpacity(MenuTitleTextColor)
       .Text(InArgs._MessageText)
       .WrapTextAt(500.0f)
       ]
      ]
     +SVerticalBox::Slot()
     .AutoHeight()
     .VAlign(VAlign_Center)
     .HAlign(HAlign_Center)
     .Padding(20.0f)
     [
      SNew( SHorizontalBox)
      +SHorizontalBox::Slot()
      .AutoWidth()
      .Padding(20.0f)
      .VAlign(VAlign_Center)
      .HAlign(HAlign_Center)
      [
       SNew( SButton )
       .ContentPadding(100)
       .OnClicked(InArgs._OnConfirmClicked)
       .Text(InArgs._ConfirmText)
       .TextStyle(FAmethystStyle::Get(), "AmethystGame.MenuHeaderTextStyle")
       .ButtonStyle(ButtonStyle)
       ]
      
      +SHorizontalBox::Slot()
      .AutoWidth()
      .Padding(20.0f)
      .VAlign(VAlign_Center)
      .HAlign(HAlign_Center)
      [
       SNew( SButton )
       .ContentPadding(100)
       .OnClicked(InArgs._OnCancelClicked)
       .Text(InArgs._CancelText)
       .TextStyle(FAmethystStyle::Get(), "AmethystGame.MenuHeaderTextStyle")
       .ButtonStyle(ButtonStyle)
       .Visibility(InArgs._CancelText.IsEmpty() == false ? EVisibility::Visible : EVisibility::Collapsed)
       ]
      ]
     ];
}

bool SAmethystConfirmationDialog::SupportsKeyboardFocus() const
{
    return true;
}

FReply SAmethystConfirmationDialog::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly, true);
}


FReply SAmethystConfirmationDialog::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	const FKey Key = KeyEvent.GetKey();
	const int32 UserIndex = KeyEvent.GetUserIndex();

	// Filter input based on the type of this dialog
	switch (DialogType)
	{
	case EAmethystDialogType::Generic:
	{
		// Ignore input from users that don't own this dialog
		if (PlayerOwner != nullptr && PlayerOwner->GetControllerId() != UserIndex)
		{
			return FReply::Unhandled();
		}
		break;
	}

	case EAmethystDialogType::ControllerDisconnected:
	{
		// Only ignore input from controllers that are bound to local users
		if (PlayerOwner != nullptr && PlayerOwner->GetGameInstance() != nullptr)
		{
			if (PlayerOwner->GetGameInstance()->FindLocalPlayerFromControllerId(UserIndex))
			{
				return FReply::Unhandled();
			}
		}
		break;
	}
	}

	// For testing on PC
	if ((Key == EKeys::Enter || Key == EKeys::Gamepad_FaceButton_Bottom) && !KeyEvent.IsRepeat())
	{
		if (OnConfirm.IsBound())
		{
			//these two cases should be combined when we move to using PlatformUserIDs rather than ControllerIDs.
#if PLATFORM_PS4
			bool bExecute = false;
			// For controller reconnection, bind the confirming controller to the owner of this dialog
			if (DialogType == EAmethystDialogType::ControllerDisconnected && PlayerOwner != nullptr)
			{
				const auto OnlineSub = IOnlineSubsystem::Get();
				if (OnlineSub)
				{
					const auto IdentityInterface = OnlineSub->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						TSharedPtr<FUniqueNetId> IncomingUserId = IdentityInterface->GetUniquePlayerId(UserIndex);
						TSharedPtr<FUniqueNetId> DisconnectedId = PlayerOwner->GetCachedUniqueNetId();

						if (*IncomingUserId == *DisconnectedId)
						{
							PlayerOwner->SetControllerId(UserIndex);
							bExecute = true;
						}
					}
				}
			}
			else
			{
				bExecute = true;
			}

			if (bExecute)
			{
				return OnConfirm.Execute();
			}
#else
			// For controller reconnection, bind the confirming controller to the owner of this dialog
			if (DialogType == EAmethystDialogType::ControllerDisconnected && PlayerOwner != nullptr)
			{
				PlayerOwner->SetControllerId(UserIndex);
			}

			return OnConfirm.Execute();
#endif
		}
	}
	else if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right)
	{
		if (OnCancel.IsBound())
		{
			return OnCancel.Execute();
		}
	}

	return FReply::Unhandled();
}
