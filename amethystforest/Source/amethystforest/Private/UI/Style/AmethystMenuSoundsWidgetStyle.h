
#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "SlateBrush.h"
#include "SlateSound.h"
#include "SlateColor.h"
#include "SlateWidgetStyle.h"
#include "AmethystMenuSoundsWidgetStyle.generated.h"

/**
 * Represents the common menu sounds used in the Amethyst game
 */
USTRUCT()
struct FAmethystMenuSoundsStyle : public FSlateWidgetStyle
{
    GENERATED_USTRUCT_BODY()
    
    FAmethystMenuSoundsStyle();
    virtual ~FAmethystMenuSoundsStyle();
    
    // FSlateWidgetStyle
    virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
    static const FName TypeName;
    virtual const FName GetTypeName() const override { return TypeName; };
static const FAmethystMenuSoundsStyle& GetDefault();

/**
 * The sound that should play when starting the game
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound StartGameSound;
FAmethystMenuSoundsStyle& SetStartGameSound(const FSlateSound& InStartGameSound) { StartGameSound = InStartGameSound; return *this; }

/**
 * The sound that should play when exiting the game
 */
UPROPERTY(EditAnywhere, Category=Sound)
FSlateSound ExitGameSound;
FAmethystMenuSoundsStyle& SetExitGameSound(const FSlateSound& InExitGameSound) { ExitGameSound = InExitGameSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UAmethystMenuSoundsWidgetStyle : public USlateWidgetStyleContainerBase
{
    GENERATED_UCLASS_BODY()
    
public:
    /** The actual data describing the sounds */
    UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
    FAmethystMenuSoundsStyle SoundsStyle;
    
    virtual const struct FSlateWidgetStyle* const GetStyle() const override
    {
        return static_cast< const struct FSlateWidgetStyle* >( &SoundsStyle );
    }
};
