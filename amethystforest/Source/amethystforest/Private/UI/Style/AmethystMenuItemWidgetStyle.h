
#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "SlateBrush.h"
#include "SlateSound.h"
#include "SlateColor.h"
#include "SlateWidgetStyle.h"
#include "AmethystMenuItemWidgetStyle.generated.h"

/**
 * Represents the appearance of an FAmethystMenuItem
 */
USTRUCT()
struct FAmethystMenuItemStyle : public FSlateWidgetStyle
{
    GENERATED_USTRUCT_BODY()
    
    FAmethystMenuItemStyle();
    virtual ~FAmethystMenuItemStyle();
    
    // FSlateWidgetStyle
    virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
    static const FName TypeName;
    virtual const FName GetTypeName() const override { return TypeName; };
static const FAmethystMenuItemStyle& GetDefault();

/**
 * The brush used for the item background
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush BackgroundBrush;
FAmethystMenuItemStyle& SetBackgroundBrush(const FSlateBrush& InBackgroundBrush) { BackgroundBrush = InBackgroundBrush; return *this; }

/**
 * The image used for the left arrow
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush LeftArrowImage;
FAmethystMenuItemStyle& SetLeftArrowImage(const FSlateBrush& InLeftArrowImage) { LeftArrowImage = InLeftArrowImage; return *this; }

/**
 * The image used for the right arrow
 */
UPROPERTY(EditAnywhere, Category=Appearance)
FSlateBrush RightArrowImage;
FAmethystMenuItemStyle& SetRightArrowImage(const FSlateBrush& InRightArrowImage) { RightArrowImage = InRightArrowImage; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UAmethystMenuItemWidgetStyle : public USlateWidgetStyleContainerBase
{
    GENERATED_UCLASS_BODY()
    
public:
    /** The actual data describing the menu's appearance. */
    UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FAmethystMenuItemStyle MenuItemStyle;
    
    virtual const struct FSlateWidgetStyle* const GetStyle() const override
    {
        return static_cast< const struct FSlateWidgetStyle* >( &MenuItemStyle );
    }
};
