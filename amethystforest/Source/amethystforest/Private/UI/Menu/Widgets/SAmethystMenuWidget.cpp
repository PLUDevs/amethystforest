
#include "amethystforest.h"
#include "Engine/Console.h"
#include "SAmethystMenuWidget.h"
#include "AmethystMenuItem.h"
#include "SAmethystMenuItem.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Style/AmethystMenuWidgetStyle.h"

#define LOCTEXT_NAMESPACE "SAmethystMenuWidget"


void SAmethystMenuWidget::Construct(const FArguments& InArgs)
{
    MenuStyle = &FAmethystStyle::Get().GetWidgetStyle<FAmethystMenuStyle>("DefaultAmethystMenuStyle");
    
    bControlsLocked = false;
    bConsoleVisible = false;
    OutlineWidth = 20.0f;
    SelectedIndex = 0;
    PlayerOwner = InArgs._PlayerOwner;
    bGameMenu = InArgs._IsGameMenu;
    ControllerHideMenuKey = EKeys::Gamepad_Special_Right;
    Visibility.Bind(this, &SAmethystMenuWidget::GetSlateVisibility);
    FLinearColor MenuTitleTextColor =  FLinearColor(FColor(155,164,182));
    MenuHeaderHeight = 62.0f;
    MenuHeaderWidth = 287.0f;
    
    UpdateMenuOwner();
    
    ChildSlot
    [
     SNew(SOverlay)
     + SOverlay::Slot()
     .HAlign(HAlign_Fill)
     .VAlign(VAlign_Fill)
     [
      SNew(SVerticalBox)
      + SVerticalBox::Slot()
      .HAlign(HAlign_Left)
      .VAlign(VAlign_Top)
      .Padding(TAttribute<FMargin>(this,&SAmethystMenuWidget::GetMenuOffset))
      [
       SNew(SVerticalBox)
       + SVerticalBox::Slot()
       .AutoHeight()
       [
        SNew(SOverlay)
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Fill)
        [
         SNew(SBox)
         .WidthOverride(MenuHeaderWidth)
         .HeightOverride(MenuHeaderHeight)
         [
          SNew(SImage)
          .ColorAndOpacity(this, &SAmethystMenuWidget::GetHeaderColor)
          .Image(&MenuStyle->HeaderBackgroundBrush)
          ]
         ]
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Fill)
        [
         SNew(SBox)
         .WidthOverride(MenuHeaderWidth)
         .HeightOverride(MenuHeaderHeight)
         .VAlign(VAlign_Center)
         .HAlign(HAlign_Center)
         [
          SNew(STextBlock)
          .TextStyle(FAmethystStyle::Get(), "AmethystGame.MenuHeaderTextStyle")
          .ColorAndOpacity(MenuTitleTextColor)
          .Text(this,&SAmethystMenuWidget::GetMenuTitle)
          ]
         ]
        ]
       + SVerticalBox::Slot()
       .AutoHeight()
       [
        SNew(SBorder)
        .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
        .ColorAndOpacity(this, &SAmethystMenuWidget::GetBottomColor)
        .VAlign(VAlign_Top)
        .HAlign(HAlign_Left)
        [
         SNew(SHorizontalBox)
         + SHorizontalBox::Slot()
         .AutoWidth()
         [
          SNew(SVerticalBox)
          + SVerticalBox::Slot()
          .AutoHeight()
          .Padding(TAttribute<FMargin>(this,&SAmethystMenuWidget::GetLeftMenuOffset))
          [
           SNew(SBorder)
           .BorderImage(&MenuStyle->LeftBackgroundBrush)
           .BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
           .Padding(FMargin(OutlineWidth))
           .DesiredSizeScale(this, &SAmethystMenuWidget::GetBottomScale)
           .VAlign(VAlign_Top)
           .HAlign(HAlign_Left)
           [
            SAssignNew(LeftBox, SVerticalBox)
            ]
           ]
          ]
         + SHorizontalBox::Slot()
         .AutoWidth()
         [
          SNew(SVerticalBox)
          + SVerticalBox::Slot()
          .Padding(TAttribute<FMargin>(this,&SAmethystMenuWidget::GetSubMenuOffset))
          .AutoHeight()
          [
           SNew(SBorder)
           .BorderImage(&MenuStyle->RightBackgroundBrush)
           .BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
           .Padding(FMargin(OutlineWidth))
           .DesiredSizeScale(this, &SAmethystMenuWidget::GetBottomScale)
           .VAlign(VAlign_Top)
           .HAlign(HAlign_Left)
           [
            SAssignNew(RightBox, SVerticalBox)
            ]
           ]
          ]
         ]
        
        ]
       ]
      
      ]
     ];
}

EVisibility SAmethystMenuWidget::GetSlateVisibility() const
{
    return bConsoleVisible ? EVisibility::HitTestInvisible : EVisibility::Visible;
}

FString SAmethystMenuWidget::GetMenuTitle() const
{
    return CurrentMenuTitle.ToString();
}

void SAmethystMenuWidget::LockControls(bool bEnable)
{
    bControlsLocked = bEnable;
}

void SAmethystMenuWidget::UpdateMenuOwner()
{
    OwnerUserIndex = PlayerOwner.IsValid() ? PlayerOwner->ControllerId : 0;
}

void SAmethystMenuWidget::BuildAndShowMenu()
{
    //grab the user settings
    UAmethystGameUserSettings* UserSettings = CastChecked<UAmethystGameUserSettings>(GEngine->GetGameUserSettings());
    ScreenRes = UserSettings->GetScreenResolution();
    
    //Build left menu panel
    bLeftMenuChanging = false;
    bGoingBack = false;
    BuildLeftPanel(bGoingBack);
    
    //sets up whole main menu animations and launches them
    SetupAnimations();
    
    // Set up right side and launch animation if there is any submenu
    if (CurrentMenu.Num() > 0 && CurrentMenu.IsValidIndex(SelectedIndex) && CurrentMenu[SelectedIndex]->bVisible)
    {
        NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
        if (NextMenu.Num() > 0)
        {
            BuildRightPanel();
            bSubMenuChanging = true;
        }
    }
    
    bMenuHiding = false;
    FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound, OwnerUserIndex);
}

void SAmethystMenuWidget::HideMenu()
{
    if (!bMenuHiding)
    {
        if(MenuWidgetAnimation.IsAtEnd())
        {
            MenuWidgetAnimation.PlayReverse();
        }
        else
        {
            MenuWidgetAnimation.Reverse();
        }
        bMenuHiding = true;
    }
}


void SAmethystMenuWidget::SetupAnimations()
{
    //Setup a curve
    const float StartDelay = 0.0f;
    const float SecondDelay = bGameMenu ? 0.f : 0.3f;
    const float AnimDuration = 0.5f;
    const float MenuChangeDuration = 0.2f;
    
    //always animate the menu from the same side of the screen; it looks silly when it disappears to one place and reappears from another
    AnimNumber = 1;
    
    MenuWidgetAnimation = FCurveSequence();
    SubMenuWidgetAnimation = FCurveSequence();
    SubMenuScrollOutCurve = SubMenuWidgetAnimation.AddCurve(0,MenuChangeDuration,ECurveEaseFunction::QuadInOut);
    
    MenuWidgetAnimation = FCurveSequence();
    LeftMenuWidgetAnimation = FCurveSequence();
    LeftMenuScrollOutCurve = LeftMenuWidgetAnimation.AddCurve(0,MenuChangeDuration,ECurveEaseFunction::QuadInOut);
    LeftMenuWidgetAnimation.Play();
    
    //Define the fade in animation
    TopColorCurve = MenuWidgetAnimation.AddCurve(StartDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
    
    //now, we want these to animate some time later
    
    //rolling out
    BottomScaleYCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
    //fading in
    BottomColorCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
    //moving from left side off screen
    ButtonsPosXCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
}

void SAmethystMenuWidget::BuildLeftPanel(bool bGoingBack)
{
    if (CurrentMenu.Num() == 0)
    {
        //do not build anything if we do not have any active menu
        return;
    }
    LeftBox->ClearChildren();
    int32 PreviousIndex = -1;
    if (bLeftMenuChanging)
    {
        if (bGoingBack && MenuHistory.Num() > 0)
        {
            FAmethystMenuInfo MenuInfo = MenuHistory.Pop();
            CurrentMenu = MenuInfo.Menu;
            CurrentMenuTitle = MenuInfo.MenuTitle;
            PreviousIndex = MenuInfo.SelectedIndex;
            if (CurrentMenu.Num() > 0 && CurrentMenu[PreviousIndex]->SubMenu.Num() > 0)
            {
                NextMenu = CurrentMenu[PreviousIndex]->SubMenu;
                bSubMenuChanging = true;
            }
        }
        else if (PendingLeftMenu.Num() > 0)
        {
            MenuHistory.Push(FAmethystMenuInfo(CurrentMenu, SelectedIndex, CurrentMenuTitle));
            CurrentMenuTitle = CurrentMenu[SelectedIndex]->GetText();
            CurrentMenu = PendingLeftMenu;
        }
    }
    SelectedIndex = PreviousIndex;
    //Setup the buttons
    for(int32 i = 0; i < CurrentMenu.Num(); ++i)
    {
        if (CurrentMenu[i]->bVisible)
        {
            TSharedPtr<SWidget> TmpWidget;
            if (CurrentMenu[i]->MenuItemType == EAmethystMenuItemType::Standard)
            {
                TmpWidget = SAssignNew(CurrentMenu[i]->Widget, SAmethystMenuItem)
                .PlayerOwner(PlayerOwner)
                .OnClicked(this, &SAmethystMenuWidget::ButtonClicked, i)
                .Text(CurrentMenu[i]->GetText().ToString())
                .bIsMultichoice(false);
            }
            else if (CurrentMenu[i]->MenuItemType == EAmethystMenuItemType::MultiChoice)
            {
                TmpWidget = SAssignNew(CurrentMenu[i]->Widget, SAmethystMenuItem)
                .PlayerOwner(PlayerOwner)
                .OnClicked(this, &SAmethystMenuWidget::ButtonClicked, i)
                .Text(CurrentMenu[i]->GetText().ToString() )
                .bIsMultichoice(true)
                .OnArrowPressed(this, &SAmethystMenuWidget::ChangeOption)
                .OptionText(this, &SAmethystMenuWidget::GetOptionText, CurrentMenu[i]);
                UpdateArrows(CurrentMenu[i]);
            }
            else if (CurrentMenu[i]->MenuItemType == EAmethystMenuItemType::CustomWidget)
            {
                TmpWidget = CurrentMenu[i]->CustomWidget;
            }
            if (TmpWidget.IsValid())
            {
                //set first selection to first valid widget
                if (SelectedIndex == -1)
                {
                    SelectedIndex = i;
                }
                LeftBox->AddSlot()	.HAlign(HAlign_Left)	.AutoHeight()
                [
                 TmpWidget.ToSharedRef()
                 ];
            }
        }
    }
    
    
    TSharedPtr<FAmethystMenuItem> FirstMenuItem = CurrentMenu.IsValidIndex(SelectedIndex) ? CurrentMenu[SelectedIndex] : NULL;
    if (FirstMenuItem.IsValid() && FirstMenuItem->MenuItemType != EAmethystMenuItemType::CustomWidget)
    {
        FirstMenuItem->Widget->SetMenuItemActive(true);
        FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
    }
}

FString SAmethystMenuWidget::GetOptionText(TSharedPtr<FAmethystMenuItem> MenuItem) const
{
    FString Result;
    if (MenuItem->SelectedMultiChoice > -1 && MenuItem->SelectedMultiChoice < MenuItem->MultiChoice.Num())
    {
        Result = MenuItem->MultiChoice[MenuItem->SelectedMultiChoice].ToString();
    }
    return Result;
}

void SAmethystMenuWidget::BuildRightPanel()
{
    RightBox->ClearChildren();
    
    if (NextMenu.Num() == 0) return;
    
    for(int32 i = 0; i < NextMenu.Num(); ++i)
    {
        if (NextMenu[i]->bVisible)
        {
            TSharedPtr<SAmethystMenuItem> TmpButton;
            //Custom menu items are not supported in the right panel
            if (NextMenu[i]->MenuItemType == EAmethystMenuItemType::Standard)
            {
                TmpButton = SAssignNew(NextMenu[i]->Widget, SAmethystMenuItem)
                .PlayerOwner(PlayerOwner)
                .Text(NextMenu[i]->GetText().ToString())
                .InactiveTextAlpha(0.3f)
                .bIsMultichoice(false);
            }
            else if (NextMenu[i]->MenuItemType == EAmethystMenuItemType::MultiChoice)
            {
                TmpButton = SAssignNew(NextMenu[i]->Widget, SAmethystMenuItem)
                .PlayerOwner(PlayerOwner)
                .Text(NextMenu[i]->GetText().ToString() )
                .InactiveTextAlpha(0.3f)
                .bIsMultichoice(true)
                .OptionText(this, &SAmethystMenuWidget::GetOptionText, NextMenu[i]);
            }
            RightBox->AddSlot()
            .HAlign(HAlign_Center)
            .AutoHeight()
            [
             TmpButton.ToSharedRef()
             ];
        }
    }
}

void SAmethystMenuWidget::UpdateArrows(TSharedPtr<FAmethystMenuItem> MenuItem)
{
    const int32 MinIndex = MenuItem->MinMultiChoiceIndex > -1 ? MenuItem->MinMultiChoiceIndex : 0;
    const int32 MaxIndex = MenuItem->MaxMultiChoiceIndex > -1 ? MenuItem->MaxMultiChoiceIndex : MenuItem->MultiChoice.Num()-1;
    const int32 CurIndex = MenuItem->SelectedMultiChoice;
    if (CurIndex > MinIndex)
    {
        MenuItem->Widget->LeftArrowVisible = EVisibility::Visible;
    }
    else
    {
        MenuItem->Widget->LeftArrowVisible = EVisibility::Collapsed;
    }
    if (CurIndex < MaxIndex)
    {
        MenuItem->Widget->RightArrowVisible = EVisibility::Visible;
    }
    else
    {
        MenuItem->Widget->RightArrowVisible = EVisibility::Collapsed;
    }
}

void SAmethystMenuWidget::EnterSubMenu()
{
    bLeftMenuChanging = true;
    bGoingBack = false;
    FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound, OwnerUserIndex);
}

void SAmethystMenuWidget::MenuGoBack(bool bSilent)
{
    if (MenuHistory.Num() > 0)
    {
        if (!bSilent)
        {
            FSlateApplication::Get().PlaySound(MenuStyle->MenuBackSound, OwnerUserIndex);
        }
        bLeftMenuChanging = true;
        bGoingBack = true;
        OnGoBack.ExecuteIfBound(CurrentMenu);
    }
    else if (bGameMenu) // only when it's in-game menu variant
    {
        if (!bSilent)
        {
            FSlateApplication::Get().PlaySound(MenuStyle->MenuBackSound, OwnerUserIndex);
        }
        OnToggleMenu.ExecuteIfBound();
    }
    else
    {
#if Amethyst_CONSOLE_UI
        // Go back to the welcome screen.
        HideMenu();
#endif
    }
}

void SAmethystMenuWidget::ConfirmMenuItem()
{
    if (CurrentMenu[SelectedIndex]->OnConfirmMenuItem.IsBound())
    {
        CurrentMenu[SelectedIndex]->OnConfirmMenuItem.Execute();
    }
    else if (CurrentMenu[SelectedIndex]->SubMenu.Num() > 0)
    {
        EnterSubMenu();
    }
}

void SAmethystMenuWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    //Always tick the super
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    
    //ugly code seeing if the console is open
    UConsole* ViewportConsole = (GEngine !=NULL && GEngine->GameViewport != NULL) ? GEngine->GameViewport->ViewportConsole : NULL;
    if (ViewportConsole != NULL && (ViewportConsole->ConsoleState == "Typing" || ViewportConsole->ConsoleState == "Open"))
    {
        if (!bConsoleVisible)
        {
            bConsoleVisible = true;
            FSlateApplication::Get().SetFocusToGameViewport();
        }
    }
    else
    {
        if (bConsoleVisible)
        {
            bConsoleVisible = false;
            FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
        }
    }
    
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportFrame)
    {
        FViewport* Viewport = GEngine->GameViewport->ViewportFrame->GetViewport();
        if (Viewport)
        {
            ScreenRes = Viewport->GetSizeXY();
        }
    }
    
    if (MenuWidgetAnimation.IsAtStart() && !bMenuHiding)
    {
        //Start the menu widget animation, set keyboard focus
        FadeIn();
    }
    else if (MenuWidgetAnimation.IsAtStart() && bMenuHiding)
    {
        bMenuHiding = false;
        //Send event, so menu can be removed
        OnMenuHidden.ExecuteIfBound();
    }
    
    if (MenuWidgetAnimation.IsAtEnd())
    {
        if (bLeftMenuChanging)
        {
            if (LeftMenuWidgetAnimation.IsAtEnd())
            {
                PendingLeftMenu = NextMenu;
                if (NextMenu.Num() > 0
                    && NextMenu.Top()->SubMenu.Num() > 0)
                {
                    NextMenu = NextMenu.Top()->SubMenu;
                }
                else
                {
                    NextMenu.Reset();
                }
                bSubMenuChanging = true;
                
                LeftMenuWidgetAnimation.PlayReverse();
            }
            if (!LeftMenuWidgetAnimation.IsPlaying())
            {
                if (CurrentMenu.Num() > 0)
                {
                    BuildLeftPanel(bGoingBack);
                    LeftMenuWidgetAnimation.Play();
                }
                //Focus the custom widget
                if (CurrentMenu.Num() == 1 && CurrentMenu.Top()->MenuItemType == EAmethystMenuItemType::CustomWidget)
                {
                    FSlateApplication::Get().SetKeyboardFocus(CurrentMenu.Top()->CustomWidget);
                }
                bLeftMenuChanging = false;
                RightBox->ClearChildren();
            }
        }
        if (bSubMenuChanging)
        {
            if (SubMenuWidgetAnimation.IsAtEnd())
            {
                SubMenuWidgetAnimation.PlayReverse();
            }
            if (!SubMenuWidgetAnimation.IsPlaying())
            {
                if (NextMenu.Num() > 0)
                {
                    BuildRightPanel();
                    SubMenuWidgetAnimation.Play();
                }
                bSubMenuChanging = false;
            }
        }
    }
}

FMargin SAmethystMenuWidget::GetMenuOffset() const
{
    const float WidgetWidth = LeftBox->GetDesiredSize().X + RightBox->GetDesiredSize().X;
    const float WidgetHeight = LeftBox->GetDesiredSize().Y + MenuHeaderHeight;
    const float OffsetX = (ScreenRes.X - WidgetWidth - OutlineWidth*2)/2;
    const float AnimProgress = ButtonsPosXCurve.GetLerp();
    FMargin Result;
    
    switch (AnimNumber)
    {
        case 0:
            Result = FMargin(OffsetX + ScreenRes.X - AnimProgress*ScreenRes.X, (ScreenRes.Y - WidgetHeight)/2, 0, 0);
            break;
        case 1:
            Result = FMargin(OffsetX - ScreenRes.X + AnimProgress*ScreenRes.X, (ScreenRes.Y - WidgetHeight)/2, 0, 0);
            break;
        case 2:
            Result = FMargin(OffsetX, (ScreenRes.Y - WidgetHeight)/2 + ScreenRes.Y - AnimProgress*ScreenRes.Y, 0, 0);
            break;
        case 3:
            Result = FMargin(OffsetX, (ScreenRes.Y - WidgetHeight)/2 + -ScreenRes.Y + AnimProgress*ScreenRes.Y, 0, 0);
            break;
    }
    return Result;
}

FMargin SAmethystMenuWidget::GetLeftMenuOffset() const
{
    const float LeftBoxSizeX = LeftBox->GetDesiredSize().X + OutlineWidth * 2;
    return FMargin(0, 0,-LeftBoxSizeX + LeftMenuScrollOutCurve.GetLerp() * LeftBoxSizeX,0);
}

FMargin SAmethystMenuWidget::GetSubMenuOffset() const
{
    const float RightBoxSizeX = RightBox->GetDesiredSize().X + OutlineWidth * 2;
    return FMargin(0, 0,-RightBoxSizeX + SubMenuScrollOutCurve.GetLerp() * RightBoxSizeX,0);
}


FVector2D SAmethystMenuWidget::GetBottomScale() const
{
    return FVector2D(BottomScaleYCurve.GetLerp(), BottomScaleYCurve.GetLerp());
}

FLinearColor SAmethystMenuWidget::GetBottomColor() const
{
    return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), BottomColorCurve.GetLerp());
}

FLinearColor SAmethystMenuWidget::GetTopColor() const
{
    return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), TopColorCurve.GetLerp());
}

FSlateColor SAmethystMenuWidget::GetHeaderColor() const
{
    return CurrentMenuTitle.IsEmpty() ? FLinearColor::Transparent : FLinearColor::White;
}

FReply SAmethystMenuWidget::ButtonClicked(int32 ButtonIndex)
{
    if (bControlsLocked)
    {
        return FReply::Handled();
    }
    
    if (SelectedIndex != ButtonIndex)
    {
        TSharedPtr<SAmethystMenuItem> MenuItem = CurrentMenu[SelectedIndex]->Widget;
        MenuItem->SetMenuItemActive(false);
        SelectedIndex = ButtonIndex;
        MenuItem = CurrentMenu[SelectedIndex]->Widget;
        MenuItem->SetMenuItemActive(true);
        NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
        bSubMenuChanging = true;
        FSlateApplication::Get().PlaySound(MenuStyle->MenuItemChangeSound, OwnerUserIndex);
    }
    else if (SelectedIndex == ButtonIndex)
    {
        ConfirmMenuItem();
    }
    
    return FReply::Handled().SetKeyboardFocus(SharedThis(this), EKeyboardFocusCause::SetDirectly);
}

void SAmethystMenuWidget::FadeIn()
{
    //Start the menu widget playing
    MenuWidgetAnimation.Play();
    
    //Go into UI mode
    FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

FReply SAmethystMenuWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    //If we clicked anywhere, jump to the end
    if(MenuWidgetAnimation.IsPlaying())
    {
        MenuWidgetAnimation.JumpToEnd();
    }
    
    //Set the keyboard focus
    return FReply::Handled()
    .SetKeyboardFocus(SharedThis(this), EKeyboardFocusCause::SetDirectly);
}

void SAmethystMenuWidget::ChangeOption(int32 MoveBy)
{
    TSharedPtr<FAmethystMenuItem> MenuItem = CurrentMenu[SelectedIndex];
    
    const int32 MinIndex = MenuItem->MinMultiChoiceIndex > -1 ? MenuItem->MinMultiChoiceIndex : 0;
    const int32 MaxIndex = MenuItem->MaxMultiChoiceIndex > -1 ? MenuItem->MaxMultiChoiceIndex : MenuItem->MultiChoice.Num()-1;
    const int32 CurIndex = MenuItem->SelectedMultiChoice;
    
    if (MenuItem->MenuItemType == EAmethystMenuItemType::MultiChoice)
    {
        if ( CurIndex + MoveBy >= MinIndex && CurIndex + MoveBy <= MaxIndex)
        {
            MenuItem->SelectedMultiChoice += MoveBy;
            MenuItem->OnOptionChanged.ExecuteIfBound(MenuItem, MenuItem->SelectedMultiChoice);
            FSlateApplication::Get().PlaySound(MenuStyle->OptionChangeSound, OwnerUserIndex);
        }
        UpdateArrows(MenuItem);
    }
}

int32 SAmethystMenuWidget::GetNextValidIndex(int32 MoveBy)
{
    int32 Result = SelectedIndex;
    if (MoveBy != 0 && SelectedIndex + MoveBy > -1 && SelectedIndex+MoveBy < CurrentMenu.Num())
    {
        Result = SelectedIndex + MoveBy;
        //look for non-hidden menu item
        while (!CurrentMenu[Result]->Widget.IsValid())
        {
            MoveBy > 0 ? Result++ : Result--;
            //when moved outside of array, just return current selection
            if (!CurrentMenu.IsValidIndex(Result))
            {
                Result = SelectedIndex;
                break;
            }
        }
    }
    return Result;
}

FReply SAmethystMenuWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyboardEvent& InKeyboardEvent)
{
    FReply Result = FReply::Unhandled();
    if (!bControlsLocked)
    {
        const FKey Key = InKeyboardEvent.GetKey();
        
        if (Key == EKeys::Up || Key == EKeys::Down)
        {
            int32 MoveBy = Key == EKeys::Up ? -1 : 1;
            int32 NextValidIndex = GetNextValidIndex(MoveBy);
            if (NextValidIndex != SelectedIndex)
            {
                ButtonClicked(NextValidIndex);
            }
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Left || Key == EKeys::Right)
        {
            int32 MoveBy = Key == EKeys::Left ? -1 : 1;
            ChangeOption(MoveBy);
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Enter)
        {
            ConfirmMenuItem();
            Result = FReply::Handled();
        } 
        else if (Key == EKeys::Escape )
        {
            MenuGoBack();
            Result = FReply::Handled();
        }
    }
    return Result;
}

FReply SAmethystMenuWidget::OnControllerButtonPressed( const FGeometry& MyGeometry, const FControllerEvent& ControllerEvent )
{
    FReply Result = FReply::Unhandled();
    const int32 UserIndex = ControllerEvent.GetUserIndex();
    bool bEventUserCanInteract = OwnerUserIndex == -1 || UserIndex == OwnerUserIndex;
    
    if (!bControlsLocked && bEventUserCanInteract)
    {
        const FKey Key = ControllerEvent.GetEffectingButton();
        if (Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
        {
            int32 NextValidIndex = GetNextValidIndex(-1);
            if (NextValidIndex != SelectedIndex)
            {
                ButtonClicked(NextValidIndex);
            }
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
        {
            int32 NextValidIndex = GetNextValidIndex(1);
            if (NextValidIndex != SelectedIndex)
            {
                ButtonClicked(NextValidIndex);
            }
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
        {
            ChangeOption(-1);
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
        {
            ChangeOption(1);
            Result = FReply::Handled();
        }
        else if (Key == EKeys::Gamepad_FaceButton_Bottom && !ControllerEvent.IsRepeat())
        {
            ConfirmMenuItem();
            Result = FReply::Handled();
        } 
        else if ((Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Left || Key == EKeys::Global_Back || Key == EKeys::Global_View) && !ControllerEvent.IsRepeat())
        {
            MenuGoBack();
            Result = FReply::Handled();
        } 
        else if ((Key == ControllerHideMenuKey || Key == EKeys::Global_Play || Key == EKeys::Global_Menu) && !ControllerEvent.IsRepeat())
        {
            OnToggleMenu.ExecuteIfBound();
            Result = FReply::Handled();
        }
    }
    return Result;
}

FReply SAmethystMenuWidget::OnKeyboardFocusReceived(const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent)
{
    //Focus the custom widget
    if (CurrentMenu.Num() == 1 && CurrentMenu.Top()->MenuItemType == EAmethystMenuItemType::CustomWidget)
    {
        return FReply::Handled().ReleaseJoystickCapture().SetKeyboardFocus(CurrentMenu.Top()->CustomWidget.ToSharedRef(),EKeyboardFocusCause::SetDirectly);
    }
    
    return FReply::Handled().ReleaseMouseCapture().CaptureJoystick(SharedThis( this ), true);
}

#undef LOCTEXT_NAMESPACE
