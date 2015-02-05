
#include "amethystforest.h"
#include "AmethystOptions.h"
#include "Classes/Player/AmethystPersistentUser.h"
#include "Classes/Game/AmethystGameState.h"
#include "Classes/AmethystTypes.h"
#include "Classes/Player/amethystforestPlayerController.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Style/AmethystOptionsWidgetStyle.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

void FAmethystOptions::Construct(ULocalPlayer* _PlayerOwner)
{
    OptionsStyle = &FAmethystStyle::Get().GetWidgetStyle<FAmethystOptionsStyle>("DefaultAmethystOptionsStyle");
    
    PlayerOwner = _PlayerOwner;
    MinSensitivity = 1;
    
    TArray<FText> ResolutionList;
    TArray<FText> OnOffList;
    TArray<FText> SensitivityList;
    TArray<FText> GammaList;
    TArray<FText> LowHighList;
    
    FDisplayMetrics DisplayMetrics;
    FSlateApplication::Get().GetInitialDisplayMetrics(DisplayMetrics);
    
    bool bAddedNativeResolution = false;
    const FIntPoint NativeResolution(DisplayMetrics.PrimaryDisplayWidth, DisplayMetrics.PrimaryDisplayHeight);
    
    for (int32 i = 0; i < DefaultAmethystResCount; i++)
    {
        if (DefaultAmethystResolutions[i].X <= DisplayMetrics.PrimaryDisplayWidth && DefaultAmethystResolutions[i].Y <= DisplayMetrics.PrimaryDisplayHeight)
        {
            ResolutionList.Add(FText::Format(FText::FromString("{0}x{1}"), FText::FromString(FString::FromInt(DefaultAmethystResolutions[i].X)), FText::FromString(FString::FromInt(DefaultAmethystResolutions[i].Y))));
            Resolutions.Add(DefaultAmethystResolutions[i]);
            
            bAddedNativeResolution = bAddedNativeResolution || (DefaultAmethystResolutions[i] == NativeResolution);
        }
    }
    
    // Always make sure that the native resolution is available
    if (!bAddedNativeResolution)
    {
        ResolutionList.Add(FText::Format(FText::FromString("{0}x{1}"), FText::FromString(FString::FromInt(NativeResolution.X)), FText::FromString(FString::FromInt(NativeResolution.Y))));
        Resolutions.Add(NativeResolution);
    }
    
    OnOffList.Add(LOCTEXT("Off","OFF"));
    OnOffList.Add(LOCTEXT("On","ON"));
    
    LowHighList.Add(LOCTEXT("Low","LOW"));
    LowHighList.Add(LOCTEXT("High","HIGH"));
    
    //Mouse sensitivity 0-50
    for (int32 i = 0; i < 51; i++)
    {
        SensitivityList.Add(FText::AsNumber(i));
    }
    
    for (int32 i = -50; i < 51; i++)
    {
        GammaList.Add(FText::AsNumber(i));
    }
    
    /** Options menu root item */
    TSharedPtr<FAmethystMenuItem> OptionsRoot = FAmethystMenuItem::CreateRoot();
    
    /** Cheats menu root item */
    TSharedPtr<FAmethystMenuItem> CheatsRoot = FAmethystMenuItem::CreateRoot();
    
    CheatsItem = MenuHelper::AddMenuItem(CheatsRoot,LOCTEXT("Cheats", "CHEATS"));
    MenuHelper::AddMenuOptionSP(CheatsItem, LOCTEXT("InfiniteAmmo", "INFINITE AMMO"), OnOffList, this, &FAmethystOptions::InfiniteAmmoOptionChanged);
    MenuHelper::AddMenuOptionSP(CheatsItem, LOCTEXT("InfiniteClip", "INFINITE CLIP"), OnOffList, this, &FAmethystOptions::InfiniteClipOptionChanged);
    MenuHelper::AddMenuOptionSP(CheatsItem, LOCTEXT("HealthRegen", "HP REGENERATION"), OnOffList, this, &FAmethystOptions::HealthRegenOptionChanged);
    
    OptionsItem = MenuHelper::AddMenuItem(OptionsRoot,LOCTEXT("Options", "OPTIONS"));
#if PLATFORM_DESKTOP
    VideoResolutionOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("Resolution", "RESOLUTION"), ResolutionList, this, &FAmethystOptions::VideoResolutionOptionChanged);
    GraphicsQualityOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("Quality", "QUALITY"),LowHighList, this, &FAmethystOptions::GraphicsQualityOptionChanged);
    FullScreenOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("FullScreen", "FULL SCREEN"),OnOffList, this, &FAmethystOptions::FullScreenOptionChanged);
#endif
    GammaOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("Gamma", "GAMMA CORRECTION"),GammaList, this, &FAmethystOptions::GammaOptionChanged);
    AimSensitivityOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("AimSensitivity", "AIM SENSITIVITY"),SensitivityList, this, &FAmethystOptions::AimSensitivityOptionChanged);
    InvertYAxisOption = MenuHelper::AddMenuOptionSP(OptionsItem,LOCTEXT("InvertYAxis", "INVERT Y AXIS"),OnOffList, this, &FAmethystOptions::InvertYAxisOptionChanged);
    MenuHelper::AddMenuItemSP(OptionsItem,LOCTEXT("ApplyChanges", "APPLY CHANGES"), this, &FAmethystOptions::OnApplySettings);
    
    //Do not allow to set aim sensitivity to 0
    AimSensitivityOption->MinMultiChoiceIndex = MinSensitivity;
    
    UserSettings = CastChecked<UAmethystGameUserSettings>(GEngine->GetGameUserSettings());
    ResolutionOpt = UserSettings->GetScreenResolution();
    bFullScreenOpt = UserSettings->GetFullscreenMode();
    GraphicsQualityOpt = UserSettings->GetGraphicsQuality();
    
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        bInvertYAxisOpt = PersistentUser->GetInvertedYAxis();
        SensitivityOpt = PersistentUser->GetAimSensitivity();
        GammaOpt = PersistentUser->GetGamma();
    }
    else
    {
        bInvertYAxisOpt = false;
        SensitivityOpt = 1.0f;
        GammaOpt = 2.2f;
    }
}

void FAmethystOptions::OnApplySettings()
{
    FSlateApplication::Get().PlaySound(OptionsStyle->AcceptChangesSound, GetOwnerUserIndex());
    ApplySettings();
}

void FAmethystOptions::ApplySettings()
{
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        PersistentUser->SetAimSensitivity(SensitivityOpt);
        PersistentUser->SetInvertedYAxis(bInvertYAxisOpt);
        PersistentUser->SetGamma(GammaOpt);
        PersistentUser->TellInputAboutKeybindings();
        
        PersistentUser->SaveIfDirty();
    }
    UserSettings->SetScreenResolution(ResolutionOpt);
    UserSettings->SetFullscreenMode(bFullScreenOpt);
    UserSettings->SetGraphicsQuality(GraphicsQualityOpt);
    UserSettings->ApplySettings(false);
     
    OnApplyChanges.ExecuteIfBound();
}

void FAmethystOptions::TellInputAboutKeybindings()
{
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        PersistentUser->TellInputAboutKeybindings();
    }
}

void FAmethystOptions::RevertChanges()
{
    FSlateApplication::Get().PlaySound(OptionsStyle->DiscardChangesSound, GetOwnerUserIndex());
    UpdateOptions();
    GEngine->DisplayGamma =  2.2f + 2.0f * (-0.5f + GammaOption->SelectedMultiChoice / 100.0f);
}

int32 FAmethystOptions::GetCurrentResolutionIndex(FIntPoint CurrentRes)
{
    int32 Result = 0; // return first valid resolution if match not found
    for (int32 i = 0; i < Resolutions.Num(); i++)
    {
        if (Resolutions[i] == CurrentRes)
        {
            Result = i;
            break;
        }
    }
    return Result;
}

int32 FAmethystOptions::GetCurrentMouseYAxisInvertedIndex()
{
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        return InvertYAxisOption->SelectedMultiChoice = PersistentUser->GetInvertedYAxis() ? 1 : 0;
    }
    else
    {
        return 0;
    }
}

int32 FAmethystOptions::GetCurrentMouseSensitivityIndex()
{
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        //mouse sensitivity is a floating point value ranged from 0.0f to 1.0f
        int32 IntSensitivity = FMath::RoundToInt((PersistentUser->GetAimSensitivity() - 0.5f) * 10.0f);
        //Clamp to valid index range
        return FMath::Clamp(IntSensitivity, MinSensitivity, 100);
    }
    
    return FMath::RoundToInt((1.0f - 0.5f) * 10.0f);
}

int32 FAmethystOptions::GetCurrentGammaIndex()
{
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        //reverse gamma calculation
        int32 GammaIndex = FMath::TruncToInt(((PersistentUser->GetGamma() - 2.2f) / 2.0f + 0.5f) * 100);
        //Clamp to valid index range
        return FMath::Clamp(GammaIndex, 0, 100);
    }
    
    return FMath::TruncToInt(((2.2f - 2.2f) / 2.0f + 0.5f) * 100);
}

int32 FAmethystOptions::GetOwnerUserIndex() const
{
    return PlayerOwner ? PlayerOwner->ControllerId : 0;
}

UAmethystPersistentUser* FAmethystOptions::GetPersistentUser() const
{
    UAmethystLocalPlayer* const SLP = Cast<UAmethystLocalPlayer>(PlayerOwner);
    if (SLP)
    {
        return SLP->GetPersistentUser();
    }
    
    return nullptr;
}

void FAmethystOptions::UpdateOptions()
{
#if UE_BUILD_SHIPPING
    CheatsItem->bVisible = false;
#else
    //Toggle Cheat menu visibility depending if we are client or server
    UWorld* const World = PlayerOwner->GetWorld();
    if (World && World->GetNetMode() == NM_Client)
    {
        CheatsItem->bVisible = false;
    }
    else
    {
        CheatsItem->bVisible = true;
    }
#endif
    
    //grab the user settings
    UAmethystPersistentUser* const PersistentUser = GetPersistentUser();
    if (PersistentUser)
    {
        // Update bInvertYAxisOpt, SensitivityOpt and GammaOpt because the AmethystOptions can be created without the controller having a player
        // by the in-game menu which will leave them with default values
        bInvertYAxisOpt = PersistentUser->GetInvertedYAxis();
        SensitivityOpt = PersistentUser->GetAimSensitivity();
        GammaOpt = PersistentUser->GetGamma();
    }
    
    InvertYAxisOption->SelectedMultiChoice =  GetCurrentMouseYAxisInvertedIndex();
    AimSensitivityOption->SelectedMultiChoice = GetCurrentMouseSensitivityIndex();
    GammaOption->SelectedMultiChoice = GetCurrentGammaIndex();
#if PLATFORM_DESKTOP
    VideoResolutionOption->SelectedMultiChoice = GetCurrentResolutionIndex(UserSettings->GetScreenResolution());
    GraphicsQualityOption->SelectedMultiChoice = UserSettings->GetGraphicsQuality();
    FullScreenOption->SelectedMultiChoice = UserSettings->GetCurrentFullscreenMode() != EWindowMode::Windowed ? 1 : 0;
#endif
    
}

void FAmethystOptions::InfiniteAmmoOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    UWorld* const World = PlayerOwner->GetWorld();
    if (World)
    {
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            AamethystforestPlayerController* AmethystPC = Cast<AamethystforestPlayerController>(*It);
            if (AmethystPC)
            {
                AmethystPC->SetInfiniteAmmo(MultiOptionIndex > 0 ? true : false);
            }
        }
    }
}

void FAmethystOptions::InfiniteClipOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    UWorld* const World = PlayerOwner->GetWorld();
    if (World)
    {
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            AamethystforestPlayerController* const AmethystPC = Cast<AamethystforestPlayerController>(*It);
            if (AmethystPC)
            {
                AmethystPC->SetInfiniteClip(MultiOptionIndex > 0 ? true : false);
            }
        }
    }
}

void FAmethystOptions::HealthRegenOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    UWorld* const World = PlayerOwner->GetWorld();
    if (World)
    {
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            AamethystforestPlayerController* const AmethystPC = Cast<AamethystforestPlayerController>(*It);
            if (AmethystPC)
            {
                AmethystPC->SetHealthRegen(MultiOptionIndex > 0 ? true : false);
            }
        }
    }
}

void FAmethystOptions::VideoResolutionOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    ResolutionOpt = Resolutions[MultiOptionIndex];
}

void FAmethystOptions::GraphicsQualityOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    GraphicsQualityOpt = MultiOptionIndex;
}

void FAmethystOptions::FullScreenOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    bFullScreenOpt = MultiOptionIndex == 0 ? EWindowMode::Windowed : EWindowMode::Fullscreen;
}

void FAmethystOptions::AimSensitivityOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    SensitivityOpt = 0.5f + (MultiOptionIndex / 10.0f);
}

void FAmethystOptions::GammaOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    GammaOpt = 2.2f + 2.0f * (-0.5f + MultiOptionIndex / 100.0f);
    GEngine->DisplayGamma = GammaOpt;
}

void FAmethystOptions::InvertYAxisOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    bInvertYAxisOpt = MultiOptionIndex > 0  ? true : false;
}

#undef LOCTEXT_NAMESPACE
