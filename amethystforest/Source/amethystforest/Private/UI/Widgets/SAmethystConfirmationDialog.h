#pragma once

#include "Slate.h"
#include "amethystforest.h"

class SAmethystConfirmationDialog : public SCompoundWidget
{
public:
    /** The player that owns the dialog. */
    ULocalPlayer* PlayerOwner;
    
    /** The delegate for confirming */
    FOnClicked OnConfirm;
    
    /** The delegate for cancelling */
    FOnClicked OnCancel;
    
    SLATE_BEGIN_ARGS( SAmethystConfirmationDialog )
    {}
    
    SLATE_ARGUMENT(ULocalPlayer*, PlayerOwner)
    
    SLATE_TEXT_ARGUMENT(MessageText)
    SLATE_TEXT_ARGUMENT(ConfirmText)
    SLATE_TEXT_ARGUMENT(CancelText)
    
    SLATE_ARGUMENT(FOnClicked, OnConfirmClicked)
    SLATE_ARGUMENT(FOnClicked, OnCancelClicked)
    
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);
    
    virtual bool SupportsKeyboardFocus() const override;
    virtual FReply OnKeyboardFocusReceived(const FGeometry& MyGeometry, const FKeyboardFocusEvent& InKeyboardFocusEvent) override;
    virtual FReply OnControllerButtonPressed(const FGeometry& MyGeometry, const FControllerEvent& ControllerEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyboardEvent& KeyboardEvent) override;
    
};
