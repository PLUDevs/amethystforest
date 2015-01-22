
#include "amethystforest.h"
#include "UI/Widgets/SAmethystConfirmationDialog.h"

UAmethystGameViewportClient::UAmethystGameViewportClient(const FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
    SetSuppressTransitionMessage(true);
}

void UAmethystGameViewportClient::NotifyPlayerAdded(int32 PlayerIndex, ULocalPlayer* AddedPlayer)
{
    Super::NotifyPlayerAdded(PlayerIndex, AddedPlayer);
    
    /* LocalPlayer
    UAmethystLocalPlayer* const AmethystLP = Cast<UAmethystLocalPlayer>(AddedPlayer);
    if (AmethystLP)
    {
        AmethystLP->LoadPersistentUser();
    }*/
}

void UAmethystGameViewportClient::AddViewportWidgetContent( TSharedRef<class SWidget> ViewportContent, const int32 ZOrder )
{
    UE_LOG( LogPlayerManagement, Log, TEXT( "UAmethystGameViewportClient::AddViewportWidgetContent: %p" ), &ViewportContent.Get() );
    
    if ( DialogWidget.IsValid() && ViewportContent != DialogWidget )
    {
        // Add to hidden list, and don't show until we hide the dialog widget
        HiddenViewportContentStack.AddUnique( ViewportContent );
        return;
    }
    
    if ( ViewportContentStack.Contains( ViewportContent ) )
    {
        return;
    }
    
    ViewportContentStack.AddUnique( ViewportContent );
    
    Super::AddViewportWidgetContent( ViewportContent, 0 );
}

void UAmethystGameViewportClient::RemoveViewportWidgetContent( TSharedRef<class SWidget> ViewportContent )
{
    UE_LOG( LogPlayerManagement, Log, TEXT( "UAmethystGameViewportClient::RemoveViewportWidgetContent: %p" ), &ViewportContent.Get()  );
    
    ViewportContentStack.Remove( ViewportContent );
    HiddenViewportContentStack.Remove( ViewportContent );
    
    Super::RemoveViewportWidgetContent( ViewportContent );
}

void UAmethystGameViewportClient::ShowDialog(ULocalPlayer* PlayerOwner, const FString & Message, const FString & Confirm, const FString & Cancel, const FOnClicked & OnConfirm, const FOnClicked & OnCancel)
{
    UE_LOG( LogPlayerManagement, Log, TEXT( "UAmethystGameViewportClient::ShowDialog..." ) );
    
    if ( DialogWidget.IsValid() )
    {
        return;	// Already showing a dialog box
    }
    
    check( HiddenViewportContentStack.Num() == 0 );
    
    TArray<TSharedRef<class SWidget>> CopyOfViewportContentStack = ViewportContentStack;
    
    // Make sure the dialog widget is the only widget visible when it's active
    for ( int32 i = ViewportContentStack.Num() - 1; i >= 0; i-- )
    {
        RemoveViewportWidgetContent( ViewportContentStack[i] );
    }
    
    HiddenViewportContentStack = CopyOfViewportContentStack;
    
    DialogWidget = SNew( SAmethystConfirmationDialog )
    .PlayerOwner(PlayerOwner)
    .MessageText(Message)
    .ConfirmText(Confirm)
    .CancelText(Cancel)
    .OnConfirmClicked(OnConfirm)
    .OnCancelClicked(OnCancel);
    
    AddViewportWidgetContent( DialogWidget.ToSharedRef() );
    
    // Remember what widget currently has focus
    OldFocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
    
    // Force focus to the dialog widget
    FSlateApplication::Get().SetKeyboardFocus( DialogWidget, EKeyboardFocusCause::SetDirectly );
}

void UAmethystGameViewportClient::HideDialog()
{
    UE_LOG( LogPlayerManagement, Log, TEXT( "UAmethystGameViewportClient::HideDialog. DialogWidget: %p, OldFocusWidget: %p" ), DialogWidget.Get(), OldFocusWidget.Get() );
    
    if ( DialogWidget.IsValid() )
    {
        const bool bRestoreOldFocus = OldFocusWidget.IsValid() && FSlateApplication::Get().GetKeyboardFocusedWidget() == DialogWidget;
        
        // Hide the dialog widget
        RemoveViewportWidgetContent( DialogWidget.ToSharedRef() );
        
        // Destroy the dialog widget
        DialogWidget = NULL;
        
        // We shouldn't have any visible widgets at this point
        check( ViewportContentStack.Num() == 0 );
        
        // Unhide all of the previously hidden widgets
        for ( int32 i = 0; i < HiddenViewportContentStack.Num(); i++ )
        {
            AddViewportWidgetContent( HiddenViewportContentStack[i] );
        }
        
        check( ViewportContentStack.Num() == HiddenViewportContentStack.Num() );
        
        // Done with these
        HiddenViewportContentStack.Empty();
        
        // Restore focus to last widget (but only if the dialog currently has focus still)
        if ( bRestoreOldFocus )
        {
            FSlateApplication::Get().SetKeyboardFocus( OldFocusWidget, EKeyboardFocusCause::SetDirectly );
        }
        
        OldFocusWidget = NULL;
    }
}

#define FORCE_FOCUS_TO_ACTIVE_WIDGET 0

#if FORCE_FOCUS_TO_ACTIVE_WIDGET
TSharedPtr<class SWidget> GetFirstKeyboardFocusableWidget( TSharedPtr<class SWidget> FocusCandidate )
{
    if ( FocusCandidate->SupportsKeyboardFocus() )
    {
        return FocusCandidate;
    }
    
    FChildren * Children = FocusCandidate->GetChildren();
    
    if ( Children != NULL )
    {
        for ( int32 i = 0; i < Children->Num(); i++ )
        {
            FocusCandidate = GetFirstKeyboardFocusableWidget( Children->GetChildAt( i ) );
            
            if ( FocusCandidate.IsValid() )
            {
                return FocusCandidate;
            }
        }
    }
    
    return NULL;
}
#endif

/* TO DO: Figure this out
bool UAmethystGameViewportClient::Tick(float DeltaSeconds)
{
    if ( HasAnyFlags( RF_ClassDefaultObject ) )
    {
        return true;
    }
    
    if ( DialogWidget.IsValid() )
    {
        // Make sure the dialog widget always has focus
        if ( FSlateApplication::Get().GetKeyboardFocusedWidget() != DialogWidget )
        {
            // Remember which widget had focus before we override it
            OldFocusWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
            
            // Force focus back to dialog
            FSlateApplication::Get().SetKeyboardFocus( DialogWidget, EKeyboardFocusCause::SetDirectly );
        }
    }
#if FORCE_FOCUS_TO_ACTIVE_WIDGET
    // This code will force the most visible widget to have focus, to simplify things
    // Have to disable for now though, since the ingame menu doesn't remove the options menu from the viewport when hidden
    // Which breaks the assumptions needed for this to work
    else if ( ViewportContentStack.Num() > 0 )
    {
        // Some widgets are wrapped with non focusable containers, find the first widget that we can focus on
        TSharedPtr<class SWidget> FocusCandidate = GetFirstKeyboardFocusableWidget( ViewportContentStack.Last() );
        
        if ( FocusCandidate.IsValid() )
        {
            if ( FSlateApplication::Get().GetKeyboardFocusedWidget() != FocusCandidate )
            {
                UE_LOG( LogPlayerManagement, Log, TEXT( "UAmethystGameViewportClient::Tick: SetKeyboardFocus. Visible: %i, Hidden: %i, Address: %p" ), ViewportContentStack.Num(), HiddenViewportContentStack.Num(), FocusCandidate.Get() );
                FSlateApplication::Get().SetKeyboardFocus( FocusCandidate, EKeyboardFocusCause::SetDirectly );
            }
        }
        else if ( !FSlateApplication::Get().GetKeyboardFocusedWidget().IsValid() || !FSlateApplication::Get().GetKeyboardFocusedWidget()->SupportsKeyboardFocus() )
        {
            FSlateApplication::Get().SetFocusToGameViewport();
        }
    }
#endif
    
    return true;
}
 */

#if WITH_EDITOR
void UAmethystGameViewportClient::DrawTransition(UCanvas* Canvas)
{
    if (GetOuterUEngine() != NULL)
    {
        TEnumAsByte<enum ETransitionType> Type = GetOuterUEngine()->TransitionType;
        switch (Type)
        {
            case TT_Connecting:
                DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "ConnectingMessage", "CONNECTING").ToString());
                break;
            case TT_WaitingToConnect:
                DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "Waitingtoconnect", "Waiting to connect...").ToString());
                break;	
        }
    }
}
#endif //WITH_EDITOR
