
#pragma once
#include "Slate.h"
#include "SAmethystMenuItem.h"

namespace EAmethystMenuItemType
{
    enum Type
    {
        Root,
        Standard,
        MultiChoice,
        CustomWidget,
    };
};

/** TArray< TSharedPtr<class FAmethystMenuItem> > */
typedef TArray< TSharedPtr<class FAmethystMenuItem> > MenuPtr;

class FAmethystMenuInfo
{
public:
    /** menu items array */
    MenuPtr Menu;
    
    /** last selection in this menu */
    int32 SelectedIndex;
    
    /** menu title */
    FText MenuTitle;
    
    /** constructor making filling required information easy */
    FAmethystMenuInfo(MenuPtr _Menu, int32 _SelectedIndex, FText _MenuTitle)
    {
        Menu = _Menu;
        SelectedIndex = _SelectedIndex;
        MenuTitle = _MenuTitle;
    }
};

class FAmethystMenuItem : public TSharedFromThis<FAmethystMenuItem>
{
public:
    /** confirm menu item delegate */
    DECLARE_DELEGATE(FOnConfirmMenuItem);
    
    /** multi-choice option changed, parameters are menu item itself and new multi-choice index  */
    DECLARE_DELEGATE_TwoParams(FOnOptionChanged, TSharedPtr<FAmethystMenuItem>, int32);
    
    /** delegate, which is executed by SAmethystMenuWidget if user confirms this menu item */
    FOnConfirmMenuItem OnConfirmMenuItem;
    
    /** multi-choice option changed, parameters are menu item itself and new multi-choice index */
    FOnOptionChanged OnOptionChanged;
    
    /** menu item type */
    EAmethystMenuItemType::Type MenuItemType;
    
    /** if this menu item will be created when menu is opened */
    bool bVisible;
    
    /** sub menu if present */
    TArray< TSharedPtr<FAmethystMenuItem> > SubMenu;
    
    /** shared pointer to actual slate widget representing the menu item */
    TSharedPtr<SAmethystMenuItem> Widget;
    
    /** shared pointer to actual slate widget representing the custom menu item, ie whole options screen */
    TSharedPtr<SWidget> CustomWidget;
    
    /** texts for multiple choice menu item (like INF AMMO ON/OFF or difficulty/resolution etc) */
    TArray<FText> MultiChoice;
    
    /** set to other value than -1 to limit the options range */
    int32 MinMultiChoiceIndex;
    
    /** set to other value than -1 to limit the options range */
    int32 MaxMultiChoiceIndex;
    
    /** selected multi-choice index for this menu item */
    int32 SelectedMultiChoice;
    
    /** constructor accepting menu item text */
    FAmethystMenuItem(FText _text)
    {
        bVisible = true;
        Text = _text;
        MenuItemType = EAmethystMenuItemType::Standard;
    }
    
    /** custom widgets cannot contain sub menus, all functionality must be handled by custom widget itself */
    FAmethystMenuItem(TSharedPtr<SWidget> _Widget)
    {
        bVisible = true;
        MenuItemType = EAmethystMenuItemType::CustomWidget;
        CustomWidget = _Widget;
    }
    
    /** constructor for multi-choice item */
    FAmethystMenuItem(FText _text, TArray<FText> _choices, int32 DefaultIndex=0)
    {
        bVisible = true;
        Text = _text;
        MenuItemType = EAmethystMenuItemType::MultiChoice;
        MultiChoice = _choices;
        MinMultiChoiceIndex = MaxMultiChoiceIndex = -1;
        SelectedMultiChoice = DefaultIndex;
    }
    
    const FText& GetText() const
    {
        return Text;
    }
    
    void SetText(const FText& UpdatedText)
    {
        Text = UpdatedText;
        if (Widget.IsValid())
        {
            Widget->UpdateItemText(Text.ToString());
        }
    }
    
    /** create special root item */
    static TSharedRef<FAmethystMenuItem> CreateRoot()
    {
        return MakeShareable(new FAmethystMenuItem());
    }
    
private:
    
    /** menu item text */
    FText Text;
    
    FAmethystMenuItem()
    {
        bVisible = false;
        MenuItemType = EAmethystMenuItemType::Root;
    }
};