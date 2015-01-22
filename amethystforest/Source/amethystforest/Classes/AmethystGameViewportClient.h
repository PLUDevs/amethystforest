
#include "Engine/GameViewportClient.h"
#include "AmethystGameViewportClient.generated.h"

UCLASS(Within=Engine, transient, config=Engine)
class UAmethystGameViewportClient : public UGameViewportClient, public FTickerObjectBase
{
    GENERATED_UCLASS_BODY()
    
public:
    
    // start UGameViewportClient interface
    void NotifyPlayerAdded( int32 PlayerIndex, ULocalPlayer* AddedPlayer ) override;
    void AddViewportWidgetContent( TSharedRef<class SWidget> ViewportContent, const int32 ZOrder = 0 ) override;
    void RemoveViewportWidgetContent( TSharedRef<class SWidget> ViewportContent ) override;
    
    void ShowDialog(ULocalPlayer* PlayerOwner, const FString & Message, const FString & Confirm, const FString & Cancel, const FOnClicked & OnConfirm, const FOnClicked & OnCancel);
    void HideDialog();
    
    //FTicker Funcs
    // TO DO: Figure out error
    //virtual bool Tick(float DeltaSeconds) override;
    
#if WITH_EDITOR
    virtual void DrawTransition(class UCanvas* Canvas) override;
#endif //WITH_EDITOR
    // end UGameViewportClient interface
    
protected:
    /** List of viewport content that the viewport is tracking */
    TArray<TSharedRef<class SWidget>>				ViewportContentStack;
    
    TArray<TSharedRef<class SWidget>>				HiddenViewportContentStack;
    
    TSharedPtr<class SWidget>						OldFocusWidget;
    
    /** Dialog widget to show temporary messages ("Controller disconnected", "Parental Controls don't allow you to play online", etc) */
    TSharedPtr<class SAmethystConfirmationDialog>	DialogWidget;
};