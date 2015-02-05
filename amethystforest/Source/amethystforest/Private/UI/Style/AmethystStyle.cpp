
#include "amethystforest.h"
#include "AmethystStyle.h"
#include "SlateGameResources.h"

TSharedPtr< FSlateStyleSet > FAmethystStyle::AmethystStyleInstance = NULL;

void FAmethystStyle::Initialize()
{
    if ( !AmethystStyleInstance.IsValid() )
    {
        AmethystStyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle( *AmethystStyleInstance );
    }
}

void FAmethystStyle::Shutdown()
{
    FSlateStyleRegistry::UnRegisterSlateStyle( *AmethystStyleInstance );
    ensure( AmethystStyleInstance.IsUnique() );
    AmethystStyleInstance.Reset();
}

FName FAmethystStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("AmethystStyle"));
    return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::GameContentDir() / "Slate"/ RelativePath + TEXT(".otf"), __VA_ARGS__ )

TSharedRef< FSlateStyleSet > FAmethystStyle::Create()
{
    TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FAmethystStyle::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
    FSlateStyleSet& Style = StyleRef.Get();
    
    // Fonts still need to be specified in code for now
    Style.Set("AmethystGame.MenuServerListTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Black", 14))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FIntPoint(-1,1))
              );
    
    Style.Set("AmethystGame.ScoreboardListTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Black", 14))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FIntPoint(-1,1))
              );
    
    Style.Set("AmethystGame.MenuTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Black", 20))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FIntPoint(-1,1))
              );
    
    Style.Set("AmethystGame.MenuHeaderTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Black", 26))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FIntPoint(-1,1))
              );
    
    Style.Set("AmethystGame.WelcomeScreen.WelcomeTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Medium", 32))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FIntPoint(-1,1))
              );
    
    Style.Set("AmethystGame.DefaultScoreboard.Row.HeaderTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Black", 24))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FVector2D(0,1))
              );
    
    Style.Set("AmethystGame.DefaultScoreboard.Row.StatTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Regular", 18))
              .SetColorAndOpacity(FLinearColor::White)
              .SetShadowOffset(FVector2D(0,1))
              );
    
    Style.Set("AmethystGame.SplitScreenLobby.StartMatchTextStyle", FTextBlockStyle()
              .SetFont(TTF_FONT("Fonts/Roboto-Regular", 16))
              .SetColorAndOpacity(FLinearColor::Green)
              .SetShadowOffset(FVector2D(0,1))
              );
    
    return StyleRef;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FAmethystStyle::ReloadTextures()
{
    FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FAmethystStyle::Get()
{
    return *AmethystStyleInstance;
}
