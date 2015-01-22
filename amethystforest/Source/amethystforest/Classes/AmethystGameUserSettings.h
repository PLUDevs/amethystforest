
#include "AmethystGameUserSettings.generated.h"

UCLASS()
class UAmethystGameUserSettings : public UGameUserSettings
{
    GENERATED_UCLASS_BODY()
    
    /** Applies all current user settings to the game and saves to permanent storage (e.g. file), optionally checking for command line overrides. */
    virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
    
    int32 GetGraphicsQuality() const
    {
        return GraphicsQuality;
    }
    
    void SetGraphicsQuality(int32 InGraphicsQuality)
    {
        GraphicsQuality = InGraphicsQuality;
    }
    
    /** Gets current fullscreen mode */
    EWindowMode::Type GetCurrentFullscreenMode() const;
    
    // interface UGameUserSettings
    virtual void SetToDefaults() override;
    
private:
    /**
     * Graphics Quality
     *	0 = Low
     *	1 = High
     */
    UPROPERTY(config)
    int32 GraphicsQuality;
    
};
