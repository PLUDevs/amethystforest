
#pragma once
#include "Slate.h"
#include "Classes/Player/AmethystPersistentUser.h"
#include "Widgets/AmethystMenuItem.h"
#include "Widgets/SAmethystMenuWidget.h"

/** supported resolutions */
const FIntPoint DefaultAmethystResolutions[] = { FIntPoint(1024,768), FIntPoint(1280,720), FIntPoint(1920,1080) };

/** supported resolutions count*/
const int32 DefaultAmethystResCount = ARRAY_COUNT(DefaultAmethystResolutions);

/** delegate called when changes are applied */
DECLARE_DELEGATE(FOnApplyChanges);

class FAmethystOptions : public TSharedFromThis<FAmethystOptions>
{
public:
    /** sets owning player controller */
    void Construct(ULocalPlayer* _PlayerOwner);
    
    /** get current options values for display */
    void UpdateOptions();
    
    /** UI callback for applying settings, plays sound */
    void OnApplySettings();
    
    /** applies changes in game settings */
    void ApplySettings();
    
    /** needed because we can recreate the subsystem that stores it */
    void TellInputAboutKeybindings();
    
    /** reverts non-saved changes in game settings */
    void RevertChanges();
    
    /** holds options menu item */
    TSharedPtr<FAmethystMenuItem> OptionsItem;
    
    /** holds cheats menu item */
    TSharedPtr<FAmethystMenuItem> CheatsItem;
    
    /** called when changes were applied - can be used to close submenu */
    FOnApplyChanges OnApplyChanges;
    
protected:
    /** User settings pointer */
    UAmethystGameUserSettings* UserSettings;
    
    /** video resolution option changed handler */
    void VideoResolutionOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** graphics quality option changed handler */
    void GraphicsQualityOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** infinite ammo option changed handler */
    void InfiniteAmmoOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** infinite clip option changed handler */
    void InfiniteClipOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** health regen option changed handler */
    void HealthRegenOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** aim sensitivity option changed handler */
    void AimSensitivityOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** invert y axis option changed handler */
    void InvertYAxisOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** full screen option changed handler */
    void FullScreenOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** gamma correction option changed handler */
    void GammaOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** try to match current resolution with selected index */
    int32 GetCurrentResolutionIndex(FIntPoint CurrentRes);
    
    /** Get current mouse y-axis inverted option index*/
    int32 GetCurrentMouseYAxisInvertedIndex();
    
    /** get current mouse sensitivity option index */
    int32 GetCurrentMouseSensitivityIndex();
    
    /** get current gamma index */
    int32 GetCurrentGammaIndex();
    
    /** get current user index out of PlayerOwner */
    int32 GetOwnerUserIndex() const;
    
    /** Get the persistence user associated with PlayerOwner*/
    UAmethystPersistentUser* GetPersistentUser() const;
    
    /** Owning player controller */
    ULocalPlayer* PlayerOwner;
    
    /** holds invert y axis option menu item */
    TSharedPtr<FAmethystMenuItem> InvertYAxisOption;
    
    /** holds aim sensitivity option menu item */
    TSharedPtr<FAmethystMenuItem> AimSensitivityOption;
    
    /** holds mouse sensitivity option menu item */
    TSharedPtr<FAmethystMenuItem> VideoResolutionOption;
    
    /** holds graphics quality option menu item */
    TSharedPtr<FAmethystMenuItem> GraphicsQualityOption;
    
    /** holds gamma correction option menu item */
    TSharedPtr<FAmethystMenuItem> GammaOption;
    
    /** holds full screen option menu item */
    TSharedPtr<FAmethystMenuItem> FullScreenOption;
    
    /** graphics quality option */
    int32 GraphicsQualityOpt;
    
    /** minimum sensitivity index */
    int32 MinSensitivity;
    
    /** current sensitivity set in options */
    float SensitivityOpt;
    
    /** current gamma correction set in options */
    float GammaOpt;
    
    /** full screen setting set in options */
    EWindowMode::Type bFullScreenOpt;
    
    /** invert mouse setting set in options */
    uint8 bInvertYAxisOpt : 1;
    
    /** resolution setting set in options */
    FIntPoint ResolutionOpt;
    
    /** available resolutions list */
    TArray<FIntPoint> Resolutions;
    
    /** style used for the Amethyst options */
    const struct FAmethystOptionsStyle *OptionsStyle;
};