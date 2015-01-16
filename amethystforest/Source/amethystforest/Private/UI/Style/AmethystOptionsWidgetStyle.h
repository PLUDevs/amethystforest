
#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "SlateBrush.h"
#include "SlateSound.h"
#include "SlateColor.h"
#include "SlateWidgetStyle.h"
#include "AmethystOptionsWidgetStyle.generated.h"

/**
 * Represents the appearance of an FAmethystOptions
 */
USTRUCT()
struct FAmethystOptionsStyle : public FSlateWidgetStyle
{
    GENERATED_USTRUCT_BODY()
    
    FAmethystOptionsStyle();
    virtual ~FAmethystOptionsStyle();
    
    // FSlateWidgetStyle
    virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
    static const FName TypeName;
    virtual const FName GetTypeName() const override { return TypeName; };
static const FAmethystOptionsStyle& GetDefault();

/**
 * The sound the options should play when changes are accepted
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound AcceptChangesSound;
FAmethystOptionsStyle& SetAcceptChangesSound(const FSlateSound& InAcceptChangesSound) { AcceptChangesSound = InAcceptChangesSound; return *this; }

/**
 * The sound the options should play when changes are discarded
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound DiscardChangesSound;
FAmethystOptionsStyle& SetDiscardChangesSound(const FSlateSound& InDiscardChangesSound) { DiscardChangesSound = InDiscardChangesSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UAmethystOptionsWidgetStyle : public USlateWidgetStyleContainerBase
{
    GENERATED_UCLASS_BODY()
    
public:
    /** The actual data describing the menu's appearance. */
    UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FAmethystOptionsStyle OptionsStyle;
    
    virtual const struct FSlateWidgetStyle* const GetStyle() const override
    {
        return static_cast< const struct FSlateWidgetStyle* >( &OptionsStyle );
    }
};
