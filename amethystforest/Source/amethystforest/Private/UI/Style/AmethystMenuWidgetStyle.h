
#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "SlateBrush.h"
#include "SlateSound.h"
#include "SlateColor.h"
#include "SlateWidgetStyle.h"
#include "AmethystMenuWidgetStyle.generated.h"

/**
 * Represents the appearance of an SAmethystMenuWidget
 */
USTRUCT()
struct FAmethystMenuStyle : public FSlateWidgetStyle
{
    GENERATED_USTRUCT_BODY()
    
    FAmethystMenuStyle();
    virtual ~FAmethystMenuStyle();
    
    // FSlateWidgetStyle
    virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
    static const FName TypeName;
    virtual const FName GetTypeName() const override { return TypeName; };
static const FAmethystMenuStyle& GetDefault();

/**
 * The brush used for the header background
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush HeaderBackgroundBrush;
FAmethystMenuStyle& SetHeaderBackgroundBrush(const FSlateBrush& InHeaderBackgroundBrush) { HeaderBackgroundBrush = InHeaderBackgroundBrush; return *this; }

/**
 * The brush used for the left side of the menu
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush LeftBackgroundBrush;
FAmethystMenuStyle& SetLeftBackgroundBrush(const FSlateBrush& InLeftBackgroundBrush) { LeftBackgroundBrush = InLeftBackgroundBrush; return *this; }

/**
 * The brush used for the right side of the menu
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush RightBackgroundBrush;
FAmethystMenuStyle& SetRightBackgroundBrush(const FSlateBrush& InRightBackgroundBrush) { RightBackgroundBrush = InRightBackgroundBrush; return *this; }

/**
 * The sound that should play when entering a sub-menu
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound MenuEnterSound;
FAmethystMenuStyle& SetMenuEnterSound(const FSlateSound& InMenuEnterSound) { MenuEnterSound = InMenuEnterSound; return *this; }

/**
 * The sound that should play when leaving a sub-menu
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound MenuBackSound;
FAmethystMenuStyle& SetMenuBackSound(const FSlateSound& InMenuBackSound) { MenuBackSound = InMenuBackSound; return *this; }

/**
 * The sound that should play when changing an option
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound OptionChangeSound;
FAmethystMenuStyle& SetOptionChangeSound(const FSlateSound& InOptionChangeSound) { OptionChangeSound = InOptionChangeSound; return *this; }

/**
 * The sound that should play when changing the selected menu item
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound MenuItemChangeSound;
FAmethystMenuStyle& SetMenuItemChangeSound(const FSlateSound& InMenuItemChangeSound) { MenuItemChangeSound = InMenuItemChangeSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UAmethystMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
    GENERATED_UCLASS_BODY()
    
public:
    /** The actual data describing the menu's appearance. */
    UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FAmethystMenuStyle MenuStyle;
    
    virtual const struct FSlateWidgetStyle* const GetStyle() const override
    {
        return static_cast< const struct FSlateWidgetStyle* >( &MenuStyle );
    }
};
