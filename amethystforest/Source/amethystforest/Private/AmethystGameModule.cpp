
#include "amethystforest.h"
#include "AmethystGameDelegates.h"

#include "UI/Style/AmethystMenuSoundsWidgetStyle.h"
#include "UI/Style/AmethystMenuWidgetStyle.h"
#include "UI/Style/AmethystMenuItemWidgetStyle.h"
#include "UI/Style/AmethystOptionsWidgetStyle.h"
#include "AssetRegistryModule.h"
#include "IAssetRegistry.h"



#include "UI/Style/AmethystStyle.h"


class FAmethystGameModule : public FDefaultGameModuleImpl
{
    virtual void StartupModule() override
    {
        InitializeAmethystGameDelegates();
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
        
        //Hot reload hack
        FSlateStyleRegistry::UnRegisterSlateStyle(FAmethystStyle::GetStyleSetName());
        FAmethystStyle::Initialize();
    }
    
    virtual void ShutdownModule() override
    {
        FAmethystStyle::Shutdown();
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAmethystGameModule, AmethystGame, "AmethystGame");

DEFINE_LOG_CATEGORY(LogAmethyst)
DEFINE_LOG_CATEGORY(LogAmethystWeapon)
