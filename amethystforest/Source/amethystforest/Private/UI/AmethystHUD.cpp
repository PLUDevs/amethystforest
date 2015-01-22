// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/UI/AmethystHUD.h"
#include "Classes/Weapon/AmethystWeapon_Instance.h"
#include "Classes/Weapon/AmethystDamageType.h"
#include "Classes/Player/amethystforestPlayerController.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

const float AAmethystHUD::MinHudScale = 0.5f;

AAmethystHUD::AAmethystHUD(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    NoAmmoFadeOutTime =  1.0f;
    HitNotifyDisplayTime = 0.75f;
    KillFadeOutTime = 2.0f;
    LastEnemyHitDisplayTime = 0.2f;
    NoAmmoNotifyTime = -NoAmmoFadeOutTime;
    LastKillTime = - KillFadeOutTime;
    LastEnemyHitTime = -LastEnemyHitDisplayTime;
    
    static ConstructorHelpers::FObjectFinder<UTexture2D> HitTextureOb(TEXT("/Game/UI/HUD/HitIndicator"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> HUDMainTextureOb(TEXT("/Game/UI/HUD/HUDMain"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> HUDAssets02TextureOb(TEXT("/Game/UI/HUD/HUDAssets02"));
    static ConstructorHelpers::FObjectFinder<UTexture2D> LowHealthOverlayTextureOb(TEXT("/Game/UI/HUD/LowHealthOverlay"));
    
    static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/UI/HUD/Roboto51"));
    static ConstructorHelpers::FObjectFinder<UFont> NormalFontOb(TEXT("/Game/UI/HUD/Roboto18"));
    HitNotifyTexture = HitTextureOb.Object;
    HUDMainTexture = HUDMainTextureOb.Object;
    HUDAssets02Texture = HUDAssets02TextureOb.Object;
    LowHealthOverlayTexture = LowHealthOverlayTextureOb.Object;
    BigFont = BigFontOb.Object;
    NormalFont = NormalFontOb.Object;
    
    HitNotifyIcon[EAmethystHudPosition::Left] = UCanvas::MakeIcon(HitNotifyTexture,  158, 831, 585, 392);
    HitNotifyIcon[EAmethystHudPosition::FrontLeft] = UCanvas::MakeIcon(HitNotifyTexture, 369, 434, 460, 378);
    HitNotifyIcon[EAmethystHudPosition::Front] = UCanvas::MakeIcon(HitNotifyTexture,  848, 284, 361, 395);
    HitNotifyIcon[EAmethystHudPosition::FrontRight] = UCanvas::MakeIcon(HitNotifyTexture,  1212, 397, 427, 394);
    HitNotifyIcon[EAmethystHudPosition::Right] = UCanvas::MakeIcon(HitNotifyTexture, 1350, 844, 547, 321);
    HitNotifyIcon[EAmethystHudPosition::BackRight] = UCanvas::MakeIcon(HitNotifyTexture, 1232, 1241, 458, 341);
    HitNotifyIcon[EAmethystHudPosition::Back] = UCanvas::MakeIcon(HitNotifyTexture,  862, 1384, 353, 408);
    HitNotifyIcon[EAmethystHudPosition::BackLeft] = UCanvas::MakeIcon(HitNotifyTexture, 454, 1251, 371, 410);
    
    KillsBg = UCanvas::MakeIcon(HUDMainTexture, 15, 16, 235, 62);
    TimePlaceBg  = UCanvas::MakeIcon(HUDMainTexture, 262, 16, 255, 62);
    PrimaryWeapBg = UCanvas::MakeIcon(HUDMainTexture, 543, 17, 441, 81);
    SecondaryWeapBg = UCanvas::MakeIcon(HUDMainTexture, 676, 111, 293, 50);
    
    DeathMessagesBg = UCanvas::MakeIcon(HUDMainTexture, 502, 177, 342, 187);
    HealthBar = UCanvas::MakeIcon(HUDAssets02Texture, 67, 212, 372, 50);
    HealthBarBg = UCanvas::MakeIcon(HUDAssets02Texture, 67, 162, 372, 50);
    
    HealthIcon = UCanvas::MakeIcon(HUDAssets02Texture, 78, 262, 28, 28);
    
    Crosshair[EAmethystCrosshairDirection::Left] = UCanvas::MakeIcon(HUDMainTexture, 43, 402, 25, 9); // left
    Crosshair[EAmethystCrosshairDirection::Right] = UCanvas::MakeIcon(HUDMainTexture, 88, 402, 25, 9); // right
    Crosshair[EAmethystCrosshairDirection::Top] = UCanvas::MakeIcon(HUDMainTexture, 74, 371, 9, 25); // top
    Crosshair[EAmethystCrosshairDirection::Bottom] = UCanvas::MakeIcon(HUDMainTexture, 74, 415, 9, 25); // bottom
    Crosshair[EAmethystCrosshairDirection::Center] = UCanvas::MakeIcon(HUDMainTexture, 75, 403, 7, 7); // center
    
    Offsets[EAmethystHudPosition::Left] = FVector2D(173,0);
    Offsets[EAmethystHudPosition::FrontLeft] = FVector2D(120,125);
    Offsets[EAmethystHudPosition::Front] = FVector2D(0,173);
    Offsets[EAmethystHudPosition::FrontRight] = FVector2D(-120,125);
    Offsets[EAmethystHudPosition::Right] = FVector2D(-173,0);
    Offsets[EAmethystHudPosition::BackRight] = FVector2D(-120,-125);
    Offsets[EAmethystHudPosition::Back] = FVector2D(0,-173);
    Offsets[EAmethystHudPosition::BackLeft] = FVector2D(120,-125);
    
    
    HitNotifyCrosshair = UCanvas::MakeIcon(HUDMainTexture, 54, 453, 50, 50);
    
    Offset = 20.0f;
    HUDLight = FColor(175,202,213,255);
    HUDDark = FColor(110,124,131,255);
    ShadowedFont.bEnableShadow = true;
}


FString AAmethystHUD::GetTimeString(float TimeSeconds)
{
    // only minutes and seconds are relevant
    const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(TimeSeconds) % 3600);
    const int32 NumMinutes = TotalSeconds / 60;
    const int32 NumSeconds = TotalSeconds % 60;
    
    const FString TimeDesc = FString::Printf(TEXT("%02d:%02d"), NumMinutes, NumSeconds);
    return TimeDesc;
}

void AAmethystHUD::DrawWeaponHUD()
{
    AAmethystCharacter* MyPawn = CastChecked<AAmethystCharacter>(GetOwningPawn());
    AAmethystWeapon* MyWeapon = MyPawn->GetWeapon();
    if (MyWeapon)
    {
        //PRIMARY WEAPON
        const float PriWeapOffsetY = 65;
        const float PriWeaponBoxWidth = 150;
        
        Canvas->SetDrawColor(FColor::White);
        const float PriWeapBgPosY =  Canvas->ClipY - Canvas->OrgY - (PriWeapOffsetY + PrimaryWeapBg.VL + Offset) * ScaleUI;
        
        //Weapon draw position
        const float PriWeapPosX = Canvas->ClipX - Canvas->OrgX - ((PriWeaponBoxWidth + MyWeapon->PrimaryIcon.UL) / 2.0f + 2 * Offset) * ScaleUI;
        const float PriWeapPosY =  Canvas->ClipY - Canvas->OrgY - (PriWeapOffsetY + (PrimaryWeapBg.VL + MyWeapon->PrimaryIcon.VL) / 2 + Offset) * ScaleUI;
        
        //Clip draw position
        const float ClipWidth = MyWeapon->PrimaryClipIcon.UL +  MyWeapon->PrimaryClipIconOffset * (MyWeapon->AmmoIconsCount-1);
        const float BoxWidth = 65.0f;
        const float PriClipPosX = PriWeapPosX - (BoxWidth + ClipWidth) * ScaleUI;
        const float PriClipPosY =  Canvas->ClipY - Canvas->OrgY - (PriWeapOffsetY + (PrimaryWeapBg.VL + MyWeapon->PrimaryClipIcon.VL) / 2 + Offset) * ScaleUI;
        
        const float LeftCornerWidth = 60;
        
        FCanvasTileItem TileItem(FVector2D( PriClipPosX - Offset * ScaleUI, PriWeapBgPosY ), PrimaryWeapBg.Texture->Resource,
                                 FVector2D( LeftCornerWidth * ScaleUI, PrimaryWeapBg.VL * ScaleUI ),	 FLinearColor::White);
        MakeUV(PrimaryWeapBg, TileItem.UV0, TileItem.UV1, PrimaryWeapBg.U, PrimaryWeapBg.V, LeftCornerWidth, PrimaryWeapBg.VL);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem( TileItem );
        
        const float RestWidth =  Canvas->ClipX - PriClipPosX - LeftCornerWidth * ScaleUI;
        TileItem.Position = FVector2D(PriClipPosX - (Offset - LeftCornerWidth) * ScaleUI, PriWeapBgPosY);
        TileItem.Size = FVector2D(RestWidth, PrimaryWeapBg.VL * ScaleUI);
        MakeUV(PrimaryWeapBg, TileItem.UV0, TileItem.UV1, PrimaryWeapBg.U + PrimaryWeapBg.UL - RestWidth / ScaleUI, PrimaryWeapBg.V, RestWidth / ScaleUI, PrimaryWeapBg.VL);
        Canvas->DrawItem( TileItem );
        
        //Drawing primary weapon icon, ammo in the clip and total spare ammo numbers
        Canvas->DrawIcon(MyWeapon->PrimaryIcon, PriWeapPosX, PriWeapPosY, ScaleUI);
        
        const float TextOffset = 12;
        float SizeX, SizeY;
        float TopTextHeight;
        FString Text = FString::FromInt(MyWeapon->GetCurrentAmmoInClip());
        
        FCanvasTextItem TextItem( FVector2D::ZeroVector, FText::GetEmpty(), BigFont, HUDDark );
        TextItem.EnableShadow( FLinearColor::Black );
        Canvas->StrLen(BigFont, Text, SizeX, SizeY);
        
        const float TopTextScale = 0.73f; // of 51pt font
        const float TopTextPosX = Canvas->ClipX - Canvas->OrgX - (PriWeaponBoxWidth + Offset * 2 + (BoxWidth + SizeX * TopTextScale) / 2.0f)  * ScaleUI;
        const float TopTextPosY = Canvas->ClipY - Canvas->OrgY - (PriWeapOffsetY + PrimaryWeapBg.VL + Offset - TextOffset / 2.0f) * ScaleUI;
        TextItem.Text = FText::FromString( Text );
        TextItem.Scale = FVector2D( TopTextScale * ScaleUI, TopTextScale * ScaleUI );
        TextItem.FontRenderInfo = ShadowedFont;
        Canvas->DrawItem( TextItem, TopTextPosX, TopTextPosY );
        TopTextHeight = SizeY * TopTextScale;
        Text = FString::FromInt(MyWeapon->GetCurrentAmmo() - MyWeapon->GetCurrentAmmoInClip());
        Canvas->StrLen(BigFont, Text, SizeX, SizeY);
        
        const float BottomTextScale = 0.49f; // of 51pt font
        const float BottomTextPosX = Canvas->ClipX - Canvas->OrgX - (PriWeaponBoxWidth + Offset * 2 + (BoxWidth + SizeX * BottomTextScale) / 2.0f) * ScaleUI;
        const float BottomTextPosY = TopTextPosY + (TopTextHeight - 0.8f * TextOffset) * ScaleUI;
        TextItem.Text = FText::FromString( Text );
        TextItem.Scale = FVector2D( BottomTextScale*ScaleUI, BottomTextScale * ScaleUI );
        TextItem.FontRenderInfo = ShadowedFont;
        Canvas->DrawItem( TextItem, BottomTextPosX, BottomTextPosY );
        
        // Drawing clip icons
        Canvas->SetDrawColor(FColor::White);
        
        const float AmmoPerIcon = MyWeapon->GetAmmoPerClip() / MyWeapon->AmmoIconsCount;
        for (int32 i = 0; i < MyWeapon->AmmoIconsCount; i++)
        {
            if ((i+1) * AmmoPerIcon > MyWeapon->GetCurrentAmmoInClip())
            {
                const float UsedPerIcon = (i+1) * AmmoPerIcon - MyWeapon->GetCurrentAmmoInClip();
                float PercentLeftInIcon = 0;
                if (UsedPerIcon < AmmoPerIcon)
                {
                    PercentLeftInIcon = (AmmoPerIcon - UsedPerIcon) / AmmoPerIcon;
                }
                const int32 Color = 128 + 128 * PercentLeftInIcon;
                Canvas->SetDrawColor(Color, Color, Color, Color);
            }
            
            const float ClipOffset = MyWeapon->PrimaryClipIconOffset * ScaleUI * i;
            Canvas->DrawIcon(MyWeapon->PrimaryClipIcon, PriClipPosX + ClipOffset, PriClipPosY, ScaleUI);
        }
        Canvas->SetDrawColor(HUDDark);
        //
        
        //SECONDARY WEAPON
        AAmethystWeapon* SecondaryWeapon = NULL;
        for (int32 i=0; i < MyPawn->GetInventoryCount(); i++)
        {
            if (MyPawn->GetInventoryWeapon(i) != MyWeapon)
            {
                SecondaryWeapon = MyPawn->GetInventoryWeapon(i);
                break;
            }
        }
        if (SecondaryWeapon)
        {
            Canvas->SetDrawColor(FColor::White);
            //offsets
            const float SecWeapOffsetY = 0;
            const float SecWeaponBoxWidth = 120;
            
            //background positioning
            const float SecWeapBgPosX = Canvas->ClipX - Canvas->OrgX - (SecondaryWeapBg.UL + Offset) * ScaleUI;
            const float SecWeapBgPosY =  Canvas->ClipY - Canvas->OrgY - (SecondaryWeapBg.VL + Offset) * ScaleUI;
            
            //weapon draw position
            const float SecWeapPosX = Canvas->ClipX - Canvas->OrgX - ((SecWeaponBoxWidth + SecondaryWeapon->SecondaryIcon.UL) / 2.0f + 2 * Offset) * ScaleUI;
            const float SecWeapPosY =  Canvas->ClipY - Canvas->OrgY - (SecWeapOffsetY + (SecondaryWeapBg.VL + SecondaryWeapon->SecondaryIcon.VL) / 2.0f + Offset) * ScaleUI;
            
            //secondary clip draw position
            const float SecClipWidth = SecondaryWeapon->SecondaryClipIcon.UL +  SecondaryWeapon->SecondaryClipIconOffset * (SecondaryWeapon->AmmoIconsCount-1);
            const float BoxWidth = 45.0f;
            const float SecClipPosX = Canvas->ClipX - Canvas->OrgX - (SecWeaponBoxWidth + BoxWidth + SecClipWidth + 2 * Offset) * ScaleUI;
            const float SecClipPosY =  Canvas->ClipY - Canvas->OrgY - (SecWeapOffsetY + (SecondaryWeapBg.VL + SecondaryWeapon->SecondaryClipIcon.VL) / 2.0f + Offset) * ScaleUI;
            
            //draw background in two parts to match number of clip icons
            const float LeftCornerWidth = 38;
            FCanvasTileItem TileItem(FVector2D(  SecClipPosX - Offset * ScaleUI, SecWeapBgPosY ), SecondaryWeapBg.Texture->Resource,
                                     FVector2D( LeftCornerWidth * ScaleUI, SecondaryWeapBg.VL * ScaleUI ), FLinearColor::White);
            MakeUV(SecondaryWeapBg, TileItem.UV0, TileItem.UV1, SecondaryWeapBg.U, SecondaryWeapBg.V, LeftCornerWidth, SecondaryWeapBg.VL);
            TileItem.BlendMode = SE_BLEND_Translucent;
            Canvas->DrawItem(TileItem);
            
            const float RestWidth =  Canvas->ClipX - SecClipPosX - LeftCornerWidth * ScaleUI;
            TileItem.Position = FVector2D(SecClipPosX - (Offset - LeftCornerWidth) * ScaleUI, SecWeapBgPosY);
            TileItem.Size = FVector2D(RestWidth, SecondaryWeapBg.VL * ScaleUI);
            MakeUV(SecondaryWeapBg, TileItem.UV0, TileItem.UV1, SecondaryWeapBg.U + SecondaryWeapBg.UL - RestWidth / ScaleUI, SecondaryWeapBg.V, RestWidth / ScaleUI, SecondaryWeapBg.VL);
            Canvas->DrawItem(TileItem);
            
            /** Drawing secondary clip **/
            const float AmmoPerIcon = SecondaryWeapon->GetAmmoPerClip() / SecondaryWeapon->AmmoIconsCount;
            for (int32 i = 0; i < SecondaryWeapon->AmmoIconsCount; i++)
            {
                if ((i+1) * AmmoPerIcon > SecondaryWeapon->GetCurrentAmmoInClip())
                {
                    const float UsedPerIcon = (i+1) * AmmoPerIcon - SecondaryWeapon->GetCurrentAmmoInClip();
                    float PercentLeftInIcon = 0;
                    if (UsedPerIcon < AmmoPerIcon)
                    {
                        PercentLeftInIcon = (AmmoPerIcon - UsedPerIcon) / AmmoPerIcon;
                    }
                    const int32 Color = 128 + 128 * PercentLeftInIcon;
                    Canvas->SetDrawColor(Color, Color, Color, Color);
                }
                
                const float ClipOffset = SecondaryWeapon->SecondaryClipIconOffset * ScaleUI * i;
                Canvas->DrawIcon(SecondaryWeapon->SecondaryClipIcon, SecClipPosX + ClipOffset, SecClipPosY, ScaleUI);
            }
            
            //Drawing secondary weapon icon, ammo in the clip and total ammo numbers
            Canvas->SetDrawColor(FColor::White);
            Canvas->DrawIcon(SecondaryWeapon->SecondaryIcon, SecWeapPosX, SecWeapPosY, ScaleUI);
            
            const float TextOffset = 10;
            float SizeX, SizeY;
            float TopTextHeight;
            FString Text = FString::FromInt(SecondaryWeapon->GetCurrentAmmo());
            
            Canvas->StrLen(BigFont,Text, SizeX, SizeY);
            const float TopTextScale = 0.53f; // of 51pt font
            TopTextHeight = SizeY * TopTextScale;
            
            const float TopTextPosX = Canvas->ClipX - Canvas->OrgX - (SecWeaponBoxWidth + Offset * 2 + (BoxWidth + SizeX * TopTextScale) / 2.0f)  * ScaleUI;
            const float TopTextPosY = SecWeapBgPosY + (SecondaryWeapBg.VL - TopTextHeight) / 2.0f * ScaleUI;
            
            TextItem.Text = FText::FromString( Text );
            TextItem.Scale = FVector2D( TopTextScale * ScaleUI, TopTextScale * ScaleUI );
            Canvas->DrawItem( TextItem, TopTextPosX, TopTextPosY );
        }
        // END OF SECONDARY WEAPON
    }
}

void AAmethystHUD::DrawHealth()
{
    AAmethystCharacter* MyPawn = Cast<AAmethystCharacter>(GetOwningPawn());
    Canvas->SetDrawColor(FColor::White);
    const float HealthPosX = (Canvas->ClipX - HealthBarBg.UL * ScaleUI) / 2;
    const float HealthPosY = Canvas->ClipY - (Offset + HealthBarBg.VL) * ScaleUI;
    Canvas->DrawIcon(HealthBarBg, HealthPosX, HealthPosY, ScaleUI);
    const float HealthAmount =  FMath::Min(1.0f,MyPawn->Health / MyPawn->GetMaxHealth());
    
    FCanvasTileItem TileItem(FVector2D(HealthPosX,HealthPosY), HealthBar.Texture->Resource,
                             FVector2D(HealthBar.UL * HealthAmount  * ScaleUI, HealthBar.VL * ScaleUI), FLinearColor::White);
    MakeUV(HealthBar, TileItem.UV0, TileItem.UV1, HealthBar.U, HealthBar.V, HealthBar.UL * HealthAmount, HealthBar.VL);
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem(TileItem);
    
    Canvas->DrawIcon(HealthIcon,HealthPosX + Offset * ScaleUI, HealthPosY + (HealthBar.VL - HealthIcon.VL) / 2.0f * ScaleUI, ScaleUI);
}

void AAmethystHUD::NotifyOutOfAmmo()
{
    NoAmmoNotifyTime = GetWorld()->GetTimeSeconds();
}

void AAmethystHUD::NotifyEnemyHit()
{
    LastEnemyHitTime = GetWorld()->GetTimeSeconds();
}

void AAmethystHUD::DrawHUD()
{
    Super::DrawHUD();
    ScaleUI = Canvas->ClipY / 1080.0f;
    
    // make any adjustments for splitscreen
    int32 SSPlayerIndex = 0;
    if ( PlayerOwner && PlayerOwner->IsSplitscreenPlayer(&SSPlayerIndex) )
    {
        ULocalPlayer* const LP = Cast<ULocalPlayer>(PlayerOwner->Player);
        if (LP)
        {
            const ESplitScreenType::Type SSType = LP->ViewportClient->GetCurrentSplitscreenConfiguration();
            
            if ( (SSType == ESplitScreenType::TwoPlayer_Horizontal) ||
                (SSType == ESplitScreenType::ThreePlayer_FavorBottom && SSPlayerIndex == 2) ||
                (SSType == ESplitScreenType::ThreePlayer_FavorTop && SSPlayerIndex == 0))
            {
                // full-width splitscreen viewports can handle same size HUD elements as full-screen viewports
                ScaleUI *= 2.f;
            }
        }
    }
    
    
    // Empty the info item array
    InfoItems.Empty();
    float TextScale = 1.0f;
    // enforce min
    ScaleUI = FMath::Max(ScaleUI, MinHudScale);
    
    AAmethystCharacter* MyPawn = Cast<AAmethystCharacter>(GetOwningPawn());
    if (MyPawn && MyPawn->IsAlive() && MyPawn->Health < MyPawn->GetMaxHealth() * MyPawn->GetLowHealthPercentage())
    {
        const float AnimSpeedModifier = 1.0f + 5.0f * (1.0f - MyPawn->Health/(MyPawn->GetMaxHealth() * MyPawn->GetLowHealthPercentage()));
        int32 EffectValue = 32 + 72 * (1.0f - MyPawn->Health/(MyPawn->GetMaxHealth() * MyPawn->GetLowHealthPercentage()));
        PulseValue += GetWorld()->GetDeltaSeconds() * AnimSpeedModifier;
        float EffectAlpha = FMath::Abs(FMath::Sin(PulseValue));
        
        float AlphaValue = ( 1.0f / 255.0f ) * ( EffectAlpha * EffectValue );
        
        // Full screen low health overlay
        Canvas->PopSafeZoneTransform();
        FCanvasTileItem TileItem( FVector2D( 0, 0 ), LowHealthOverlayTexture->Resource, FVector2D( Canvas->ClipX, Canvas->ClipY ), FLinearColor( 1.0f, 0.0f, 0.0f, AlphaValue ) );
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem( TileItem );
        Canvas->ApplySafeZoneTransform();
    }
    
    // net mode
    if (GetNetMode() != NM_Standalone)
    {
        FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");
        DrawDebugInfoString(NetModeDesc, Canvas->OrgX + Offset*ScaleUI, Canvas->OrgY + 5*Offset*ScaleUI, true, true, HUDLight);
    }

    float MessageOffset = (Canvas->ClipY / 4.0)* ScaleUI;

    // Render the info messages such as wating to respawn - these will be drawn below any 'killed player' message.
    ShowInfoItems(MessageOffset , ScaleUI, 1.0f);
    
}

void AAmethystHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor)
{
#if !UE_BUILD_SHIPPING
    float SizeX, SizeY;
    Canvas->StrLen(NormalFont, Text, SizeX, SizeY);
    
    const float UsePosX = bAlignLeft ? PosX : PosX - SizeX;
    const float UsePosY = bAlignTop ? PosY : PosY - SizeY;
    
    const float BoxPadding = 5.0f;
    
    FColor DrawColor(HUDDark.R, HUDDark.G, HUDDark.B, HUDDark.A * 0.2f);
    const float X = UsePosX - BoxPadding * ScaleUI;
    const float Y = UsePosY - BoxPadding * ScaleUI;
    FCanvasTileItem TileItem( FVector2D( X, Y ), FVector2D( (SizeX + BoxPadding * 2.0f) * ScaleUI, (SizeY + BoxPadding * 2.0f) * ScaleUI ), DrawColor );
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem( TileItem );
    
    FCanvasTextItem TextItem( FVector2D( UsePosX, UsePosY), FText::FromString( Text ), NormalFont, TextColor );
    TextItem.EnableShadow( FLinearColor::Black );
    TextItem.FontRenderInfo = ShadowedFont;
    TextItem.Scale = FVector2D( ScaleUI, ScaleUI );
    Canvas->DrawItem( TextItem );
#endif
}

void AAmethystHUD::DrawCrosshair()
{
    AamethystforestPlayerController* PCOwner = Cast<AamethystforestPlayerController>(PlayerOwner);
    if (PCOwner)
    {
        AAmethystCharacter* Pawn =  Cast<AAmethystCharacter>(PCOwner->GetPawn());
        if (Pawn && Pawn->GetWeapon() && !Pawn->IsRunning()
            && (Pawn->IsTargeting() || (!Pawn->IsTargeting() && !Pawn->GetWeapon()->bHideCrosshairWhileNotAiming)))
        {
            const float SpreadMulti = 300;
            AAmethystWeapon_Instance* InstantWeapon = Cast<AAmethystWeapon_Instance>(Pawn->GetWeapon());
            AAmethystWeapon* MyWeapon = Pawn->GetWeapon();
            const float CurrentTime = GetWorld()->GetTimeSeconds();
            
            float AnimOffset = 0;
            if (MyWeapon)
            {
                const float EquipStartedTime = MyWeapon->GetEquipStartedTime();
                const float EquipDuration = MyWeapon->GetEquipDuration();
                AnimOffset = 300 * (1.0f - FMath::Min(1.0f,(CurrentTime - EquipStartedTime)/EquipDuration));
            }
            float CrossSpread = 2 + AnimOffset;
            if (InstantWeapon != NULL)
            {
                CrossSpread += SpreadMulti*FMath::Tan(FMath::DegreesToRadians(InstantWeapon->GetCurrentSpread()));
            }
            float CenterX = Canvas->ClipX / 2;
            float CenterY = Canvas->ClipY / 2;
            Canvas->SetDrawColor(255,255,255,192);
            
            FCanvasIcon* CurrentCrosshair[5];
            for (int32 i=0; i< 5; i++)
            {
                if (MyWeapon && MyWeapon->UseCustomAimingCrosshair && Pawn->IsTargeting())
                {
                    CurrentCrosshair[i] = &MyWeapon->AimingCrosshair[i];
                }
                else if (MyWeapon && MyWeapon->UseCustomCrosshair)
                {
                    CurrentCrosshair[i] = &MyWeapon->Crosshair[i];
                }
                else
                {
                    CurrentCrosshair[i] = &Crosshair[i];
                }
            }
            
            if (Pawn->IsTargeting() && MyWeapon->UseLaserDot)
            {
                Canvas->SetDrawColor(255,0,0,192);
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Center],
                                 CenterX - (*CurrentCrosshair[EAmethystCrosshairDirection::Center]).UL*ScaleUI / 2.0f,
                                 CenterY - (*CurrentCrosshair[EAmethystCrosshairDirection::Center]).VL*ScaleUI / 2.0f, ScaleUI);
            }
            else
            {
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Center],
                                 CenterX - (*CurrentCrosshair[EAmethystCrosshairDirection::Center]).UL*ScaleUI / 2.0f,
                                 CenterY - (*CurrentCrosshair[EAmethystCrosshairDirection::Center]).VL*ScaleUI / 2.0f, ScaleUI);
                
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Left],
                                 CenterX - 1 - (*CurrentCrosshair[EAmethystCrosshairDirection::Left]).UL * ScaleUI - CrossSpread * ScaleUI,
                                 CenterY - (*CurrentCrosshair[EAmethystCrosshairDirection::Left]).VL*ScaleUI / 2.0f, ScaleUI);
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Right],
                                 CenterX + CrossSpread * ScaleUI,
                                 CenterY - (*CurrentCrosshair[EAmethystCrosshairDirection::Right]).VL * ScaleUI / 2.0f, ScaleUI);
                
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Top],
                                 CenterX - (*CurrentCrosshair[EAmethystCrosshairDirection::Top]).UL * ScaleUI / 2.0f,
                                 CenterY - 1 - (*CurrentCrosshair[EAmethystCrosshairDirection::Top]).VL * ScaleUI - CrossSpread * ScaleUI, ScaleUI);
                Canvas->DrawIcon(*CurrentCrosshair[EAmethystCrosshairDirection::Bottom],
                                 CenterX - (*CurrentCrosshair[EAmethystCrosshairDirection::Bottom]).UL * ScaleUI / 2.0f,
                                 CenterY + CrossSpread * ScaleUI, ScaleUI);
            }
            
            if (CurrentTime - LastEnemyHitTime >= 0 && CurrentTime - LastEnemyHitTime <= LastEnemyHitDisplayTime)
            {
                const float Alpha = FMath::Min(1.0f, 1 - (CurrentTime - LastEnemyHitTime) / LastEnemyHitDisplayTime);
                Canvas->SetDrawColor(255,255,255,255*Alpha);
                
                Canvas->DrawIcon(HitNotifyCrosshair,
                                 CenterX - HitNotifyCrosshair.UL*ScaleUI / 2.0f,
                                 CenterY - HitNotifyCrosshair.VL*ScaleUI / 2.0f, ScaleUI);
            }
        }
    }
}

void AAmethystHUD::DrawDeathMessages()
{
    if (PlayerOwner == NULL)
    {
        return;
    }
    
    float OffsetX = 20;
    float OffsetY = 20;
    
    Canvas->SetDrawColor(FColor::White);
    float DeathMsgsPosX = Canvas->OrgX + Offset * ScaleUI;
    float DeathMsgsPosY = Canvas->ClipY - (OffsetY + DeathMessagesBg.VL) * ScaleUI;
    FVector Scale(ScaleUI, ScaleUI, 0.f);
    // hardcoded value to make sure the box is big enough to hold 16 W's for both players' names
    Scale.X *= 1.85;
    Canvas->DrawScaledIcon(DeathMessagesBg, DeathMsgsPosX, DeathMsgsPosY, Scale);
    
    const FColor BlueTeamColor = FColor(70, 70, 152, 255);
    const FColor RedTeamColor = FColor(152, 70, 70, 255);
    const FColor OwnerColor = HUDLight;
    
    const FString KilledText = LOCTEXT("killed"," killed ").ToString();
    FVector2D KilledTextSize(0.0f, 0.0f);
    
    const float GameTime = GetWorld()->GetTimeSeconds();
    const float LinePadding = 6.0f;
    const float BoxPadding = 2.0f;
    const float MaxLineX = 300.0f;
    const float InitialX = Offset * 2.0f * ScaleUI;
    const float InitialY = DeathMsgsPosY + (DeathMessagesBg.VL - Offset * 2.5f) * ScaleUI ;
    
    // draw messages
    float CurrentY = InitialY;
    
    Canvas->StrLen(NormalFont, KilledText, KilledTextSize.X, KilledTextSize.Y);
    
    FCanvasTextItem TextItem( FVector2D::ZeroVector, FText::GetEmpty(), NormalFont, HUDDark );
    TextItem.EnableShadow( FLinearColor::Black );
    for (int32 i = DeathMessages.Num() - 1; i >= 0; i--)
    {
        const FDeathMessage& Message = DeathMessages[i];
        float CurrentX = InitialX;
        FVector2D KillerSize(0.0f, 0.0f);
        FVector2D VictimNameSize(0.0f, 0.0f);
        float TextScale = 1.00f;
        Canvas->StrLen(NormalFont, Message.KillerDesc, KillerSize.X, KillerSize.Y);
        TextItem.Scale = FVector2D( TextScale * ScaleUI, TextScale * ScaleUI );
        TextItem.FontRenderInfo = ShadowedFont;
        TextItem.SetColor(Message.bKillerIsOwner == true ? HUDLight : ( Message.KillerTeamNum == 0 ? RedTeamColor : BlueTeamColor));
        
        TextItem.Text = FText::FromString( Message.KillerDesc );
        Canvas->DrawItem(TextItem, CurrentX, CurrentY);
        CurrentX += KillerSize.X * TextScale * ScaleUI;
        
        if (Message.DamageType.IsValid())
        {
            Canvas->SetDrawColor(FColor::White);
            float ItemSizeY = KilledTextSize.Y * TextScale * ScaleUI;
            Canvas->DrawIcon(Message.DamageType->KillIcon,
                             CurrentX + (OffsetX / 4.0f) * ScaleUI,
                             CurrentY + (ItemSizeY - Message.DamageType->KillIcon.VL * ScaleUI) / 2.0f, ScaleUI);
            CurrentX += (Message.DamageType->KillIcon.UL + OffsetX / 2.0f) * ScaleUI;
        }
        else
        {
            TextItem.Text = FText::FromString( KilledText );
            TextItem.Scale = FVector2D( TextScale * ScaleUI, TextScale * ScaleUI );
            TextItem.FontRenderInfo = ShadowedFont;
            TextItem.SetColor(HUDDark);
            Canvas->DrawItem( TextItem, CurrentX, CurrentY );
            
            CurrentX += KilledTextSize.X * TextScale * ScaleUI;
        }
        
        TextItem.SetColor(Message.bVictimIsOwner == true ? HUDLight : (Message.VictimTeamNum == 0 ? RedTeamColor : BlueTeamColor));
        
        Canvas->StrLen(NormalFont, Message.VictimDesc, VictimNameSize.X, VictimNameSize.Y);
        TextItem.Text = FText::FromString( Message.VictimDesc );
        Canvas->DrawItem( TextItem, CurrentX, CurrentY );
        CurrentY -= (KilledTextSize.Y + LinePadding) * TextScale * ScaleUI;
    }
}

void AAmethystHUD::NotifyHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator)
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    AAmethystCharacter* MyPawn = (PlayerOwner) ? Cast<AAmethystCharacter>(PlayerOwner->GetPawn()) : NULL;
    if (MyPawn)
    {
        if (CurrentTime - LastHitTime > HitNotifyDisplayTime)
        {
            for (uint8 i = 0; i < 8; i++)
            {
                HitNotifyData[i].HitPercentage = 0;
            }
        }
        
        FVector ImpulseDir;
        FHitResult Hit;
        DamageEvent.GetBestHitInfo(this, PawnInstigator, Hit, ImpulseDir);
        
        //check hit vector against pre-defined direction vectors - left, front, right, back
        const FVector HitVector = FRotationMatrix(PlayerOwner->GetControlRotation()).InverseTransformVector(-ImpulseDir);
        
        FVector Dirs2[8] = {
            FVector(0,-1,0) /*left*/,
            FVector(1,-1,0) /*front left*/,
            FVector(1,0,0) /*front*/,
            FVector(1,1,0) /*front right*/,
            FVector(0,1,0) /*right*/,
            FVector(-1,1,0) /*back right*/,
            FVector(-1,0,0), /*back*/
            FVector(-1,-1,0) /*back left*/
        };
        int32 DirIndex = -1;
        float HighestModifier = 0;
        
        for (uint8 i = 0; i < 8; i++)
        {
            //Normalize our direction vectors
            Dirs2[i].Normalize();
            const float DirModifier = FMath::Max(0.0f, FVector::DotProduct(Dirs2[i], HitVector));
            if (DirModifier > HighestModifier)
            {
                DirIndex = i;
                HighestModifier = DirModifier;
            }
        }
        if (DirIndex > -1)
        {
            const float DamageTakenPercentage = (DamageTaken / MyPawn->Health);
            HitNotifyData[DirIndex].HitPercentage += DamageTakenPercentage * 2;
            HitNotifyData[DirIndex].HitPercentage = FMath::Clamp(HitNotifyData[DirIndex].HitPercentage, 0.0f, 1.0f);
            HitNotifyData[DirIndex].HitTime = CurrentTime;
        }
        
    }
    
    LastHitTime = CurrentTime;
}


void AAmethystHUD::DrawHitIndicator()
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastHitTime <= HitNotifyDisplayTime)
    {
        const float StartX = Canvas->ClipX / 2.0f;
        const float StartY = Canvas->ClipY / 2.0f;
        
        for (uint8 i = 0; i < 8; i++)
        {
            const float TimeModifier = FMath::Max(0.0f, 1 - (CurrentTime - HitNotifyData[i].HitTime) / HitNotifyDisplayTime);
            const float Alpha = TimeModifier * HitNotifyData[i].HitPercentage;
            Canvas->SetDrawColor(255, 255, 255, FMath::Clamp(FMath::TruncToInt(Alpha * 255 * 1.5f), 0, 255));
            Canvas->DrawIcon(HitNotifyIcon[i],
                             StartX + (HitNotifyIcon[i].U - HitNotifyTexture->GetSizeX() / 2 + Offsets[i].X) * ScaleUI,
                             StartY + (HitNotifyIcon[i].V - HitNotifyTexture->GetSizeY() / 2 + Offsets[i].Y) * ScaleUI,
                             ScaleUI);
        }
    }
}

void AAmethystHUD::MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL)
{
    if (Icon.Texture)
    {
        const float Width = Icon.Texture->GetSurfaceWidth();
        const float Height = Icon.Texture->GetSurfaceHeight();
        UV0 = FVector2D(U / Width, V / Height);
        UV1 = UV0 + FVector2D(UL / Width, VL / Height);
    }
}

void AAmethystHUD::AddMatchInfoString(const FCanvasTextItem InInfoItem )
{
    InfoItems.Add(InInfoItem);
}

float AAmethystHUD::ShowInfoItems(float YOffset, float ScaleUI, float TextScale)
{
    float Y = YOffset;
    float CanvasCentre = Canvas->ClipX / 2.0f;
    
    for (int32 iItem = 0; iItem < InfoItems.Num() ; iItem++)
    {
        float X = 0.0f;
        float SizeX, SizeY; 
        Canvas->StrLen(InfoItems[iItem].Font, InfoItems[iItem].Text.ToString(), SizeX, SizeY);
        X = CanvasCentre - ( SizeX * InfoItems[iItem].Scale.X)/2.0f;
        Canvas->DrawItem(InfoItems[iItem], X, Y);
        Y += SizeY * InfoItems[iItem].Scale.Y;
    }
    return Y;
}

#undef LOCTEXT_NAMESPACE



