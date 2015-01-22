
#include "amethystforest.h"

UAmethystGameUserSettings::UAmethystGameUserSettings(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
    SetToDefaults();
}

void UAmethystGameUserSettings::SetToDefaults()
{
    Super::SetToDefaults();
    
    GraphicsQuality = 1;
}

void UAmethystGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
    if (GraphicsQuality == 0)
    {
        ScalabilityQuality.SetFromSingleQualityLevel(1);
    }
    else
    {
        ScalabilityQuality.SetFromSingleQualityLevel(3);
    }
    
    Super::ApplySettings(bCheckForCommandLineOverrides);
    
    if (!GEngine)
    {
        return;
    }
}

int32 AmethystGameGetBoundFullScreenModeCVar()
{
    static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FullScreenMode"));
    
    if (FPlatformProperties::SupportsWindowedMode())
    {
        int32 Value = CVar->GetValueOnGameThread();
        
        if(Value >= 0 && Value <= 2)
        {
            return Value;
        }
    }
    
    // every other value behaves like 0
    return 0;
}

// depending on bFullscreen and the console variable r.FullScreenMode
EWindowMode::Type AmethystGameGetWindowModeType(bool bFullscreen)
{
    int32 FullScreenMode = AmethystGameGetBoundFullScreenModeCVar();
    
    if (FPlatformProperties::SupportsWindowedMode())
    {
        if(!bFullscreen)
        {
            return EWindowMode::Windowed;
        }
        
        if(FullScreenMode == 1 || FullScreenMode == 2)
        {
            return EWindowMode::WindowedFullscreen;
        }
    }
    
    return EWindowMode::Fullscreen;
}

EWindowMode::Type UAmethystGameUserSettings::GetCurrentFullscreenMode() const
{
    EWindowMode::Type CurrentFullscreenMode = EWindowMode::Windowed;
    if ( GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportFrame )
    {
        bool bIsCurrentlyFullscreen = GEngine->GameViewport->IsFullScreenViewport();
        CurrentFullscreenMode = AmethystGameGetWindowModeType(bIsCurrentlyFullscreen);
    }
    return CurrentFullscreenMode;
}
