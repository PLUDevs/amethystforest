
#include "amethystforest.h"
#include "AmethystWelcomeMenu.h"
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
    
    SLATE_BEGIN_ARGS( SAmethystWelcomeMenuWidget )
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
        return FSlateColor(FMath::Lerp(FLinearColor(0.0f,0.0f,0.0f,1.0f), FLinearColor(0.5f,0.5f,0.5f,1.0f), TextColorCurve.GetLerp()));
    }
    
    void Construct( const FArguments& InArgs )
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
          SNew( STextBlock )
#if PLATFORM_PS4
          .Text( LOCTEXT("PressStartPS4", "PRESS CROSS BUTTON TO PLAY" ) )
#else
          .Text( LOCTEXT("PressStartXboxOne", "PRESS A TO PLAY" ) )
#endif
          .ColorAndOpacity(this, &SAmethystWelcomeMenuWidget::GetTextColor)
          .TextStyle( FAmethystStyle::Get(), "AmethystGame.WelcomeScreen.WelcomeTextStyle" )
          ]
         ];
    }
    
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
    {
        if(!TextAnimation.IsPlaying())
        {
            if(TextAnimation.IsAtEnd())
            {
                TextAnimation.PlayReverse();
            }
            else
            {
                TextAnimation.Play();
            }
        }
    }
    
    virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyboardEvent& InKeyboardEvent ) override
    {
        return FReply::Handled();
    }
    
    virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override
    {
        return FReply::Handled();
    }
    
    virtual FReply OnControllerButtonPressed( const FGeometry& MyGeometry, const FControllerEvent& ControllerEvent ) override
    {
        const FKey Key = ControllerEvent.GetEffectingButton();
        if (Key == EKeys::Gamepad_FaceButton_Bottom)
        {
            MenuOwner->SetControllerAndAdvanceToMainMenu(ControllerEvent.GetUserIndex());
            
            return FReply::Handled();
        }
        
        return FReply::Unhandled();
    }
    
    virtual FReply OnKeyboardFocusReceived(const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent) override
    {
        return FReply::Handled().ReleaseMouseCapture().CaptureJoystick(SharedThis( this ), true);
    }
};

void FAmethystWelcomeMenu::Construct(ULocalPlayer* InPlayerOwner)
{
    PlayerOwner = InPlayerOwner;
    PendingControllerIndex = 0;
    
    MenuWidget = SNew( SAmethystWelcomeMenuWidget )
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

void FAmethystWelcomeMenu::HandleLoginUIClosed(TSharedPtr<FUniqueNetId> UniqueId, const int ControllerIndex)
{
    PendingControllerIndex = ControllerIndex;
    
    if(UniqueId.IsValid())
    {
        SetControllerAndAdvanceToMainMenu(ControllerIndex);
    }
    /*
     TO DO: AmethystGameViewportClient needed?
    else
    {
        // Show a warning that your progress won't be saved if you continue without logging in.
        UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(PlayerOwner->ViewportClient);
        
        if (AmethystViewport != NULL)
        {
            AmethystViewport->ShowDialog(
                                        PlayerOwner,
                                        FString("If you continue without signing in, your progress will not be saved."),
                                        FString("A - Continue"),
                                        FString("B - Back"),
                                        FOnClicked::CreateRaw(this, &FAmethystWelcomeMenu::OnContinueWithoutSavingConfirm),
                                        FOnClicked::CreateRaw(this, &FAmethystWelcomeMenu::OnContinueWithoutSavingBack)
                                        );
        }
    }
    */
}

void FAmethystWelcomeMenu::SetControllerAndAdvanceToMainMenu(const int ControllerIndex)
{
    if (PlayerOwner)
    {
        PlayerOwner->SetControllerId(ControllerIndex);
        
        // tell gameinstance to transition to main menu
        /* TO DO AmethystGameInstance
        UAmethystGameInstance* const SGI = Cast<UAmethystGameInstance>(PlayerOwner->GetGameInstance());
        if (SGI)
        {
            SGI->GotoState(AmethystGameInstanceState::MainMenu);
        }
         */
    }	
}

FReply FAmethystWelcomeMenu::OnContinueWithoutSavingConfirm()
{
    /*TO DO: AmethystGameViewportClient needed?
    UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(PlayerOwner->ViewportClient);
    
    if (AmethystViewport != NULL)
    {
        AmethystViewport->HideDialog();
    }
    */
    SetControllerAndAdvanceToMainMenu(PendingControllerIndex);
    return FReply::Handled();
}

FReply FAmethystWelcomeMenu::OnContinueWithoutSavingBack()
{
    /*TO DO: AmethystGameViewportClient needed?
    UAmethystGameViewportClient * AmethystViewport = Cast<UAmethystGameViewportClient>(PlayerOwner->ViewportClient);
    
    if (AmethystViewport != NULL)
    {
        AmethystViewport->HideDialog();
    }
     */
    
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
