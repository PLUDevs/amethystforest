
#pragma once

#include "Slate.h"
#include "SlateBrush.h"
#include "SlateSound.h"
#include "SlateColor.h"
#include "SlateWidgetStyle.h"

class FAmethystStyle
{
public:
    
    static void Initialize();
    
    static void Shutdown();
    
    /** reloads textures used by slate renderer */
    static void ReloadTextures();
    
    /** @return The Slate style set for the Amethyst game */
    static const ISlateStyle& Get();
    
    static FName GetStyleSetName();
    
private:
    
    static TSharedRef< class FSlateStyleSet > Create();
    
private:
    
    static TSharedPtr< class FSlateStyleSet > AmethystStyleInstance;
};