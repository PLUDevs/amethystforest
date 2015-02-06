
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

FReply SAmethystConfirmationDialog::OnKeyboardFocusReceived(const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent)
{
    return FReply::Handled().ReleaseMouseCapture().CaptureJoystick(SharedThis( this ), true);
}

FReply SAmethystConfirmationDialog::OnControllerButtonPressed( const FGeometry& MyGeometry, const FControllerEvent& ControllerEvent )
{
    const FKey Key = ControllerEvent.GetEffectingButton();
    const int32 UserIndex = ControllerEvent.GetUserIndex();
    
    if (Key == EKeys::Gamepad_FaceButton_Bottom)
    {
        if(OnConfirm.IsBound())
        {
            return OnConfirm.Execute();
        }
    }
    else if (Key == EKeys::Gamepad_FaceButton_Right)
    {
        if(OnCancel.IsBound())
        {
            return OnCancel.Execute();
        }
    }
    
    return FReply::Unhandled();
}

FReply SAmethystConfirmationDialog::OnKeyDown(const FGeometry& MyGeometry, const FKeyboardEvent& KeyboardEvent)
{
    // For testing on PC
    if (KeyboardEvent.GetKey() == EKeys::Enter)
    {
        if(OnConfirm.IsBound())
        {
            return OnConfirm.Execute();
        }
    }
    else if (KeyboardEvent.GetKey() == EKeys::Escape)
    {
        if(OnCancel.IsBound())
        {
            return OnCancel.Execute();
        }
    }
    
    return FReply::Unhandled();
}
