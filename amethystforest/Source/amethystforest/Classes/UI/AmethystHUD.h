// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "AmethystTypes.h"
#include "AmethystHUD.generated.h"

struct FHitData
{
    /** Last hit time. */
    float HitTime;
    
    /** strength of hit icon */
    float HitPercentage;
    
    /** Initialise defaults. */
    FHitData()
    {
        HitTime = 0.0f;
        HitPercentage = 0.0f;
    }
};

struct FDeathMessage
{
    /** Name of player scoring kill. */
    FString KillerDesc;
    
    /** Name of killed player. */
    FString VictimDesc;
    
    /** Killer is local player. */
    uint8 bKillerIsOwner : 1;
    
    /** Victim is local player. */
    uint8 bVictimIsOwner : 1;
    
    /** Team number of the killer. */
    int32 KillerTeamNum;
    
    /** Team number of the victim. */
    int32 VictimTeamNum;
    
    /** timestamp for removing message */
    float HideTime;
    
    /** What killed the player. */
    TWeakObjectPtr<class UAmethystDamageType> DamageType;
    
    /** Initialise defaults. */
    FDeathMessage()
    : bKillerIsOwner(false)
    , bVictimIsOwner(false)
    , KillerTeamNum(0)
    , VictimTeamNum(0)
    , HideTime(0.f)
    {
    }
};

UCLASS()
class AAmethystHUD : public AHUD
{
    GENERATED_UCLASS_BODY()
    
public:
    
    /** Main HUD update loop. */
    virtual void DrawHUD() override;
    
    /**
     * Sent from pawn hit, used to calculate hit notification overlay for drawing.
     *
     * @param	DamageTaken		The amount of damage.
     * @param	DamageEvent		The actual damage event.
     * @param	PawnInstigator	The pawn that did the damage.
     */
    void NotifyHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator);
    
    /** Sent from AmethystWeapon, shows NO AMMO text. */
    void NotifyOutOfAmmo();
    
    /** Notifies we have hit the enemy. */
    void NotifyEnemyHit();
    
protected:
    /** Floor for automatic hud scaling. */
    static const float MinHudScale;
    
    /** Lighter HUD color. */
    FColor HUDLight;
    
    /** Darker HUD color. */
    FColor HUDDark;
    
    /** When we got last notice about out of ammo. */
    float NoAmmoNotifyTime;
    
    /** How long notice is fading out. */
    float NoAmmoFadeOutTime;
    
    /** Most recent hit time, used to check if we need to draw hit indicator at all. */
    float LastHitTime;
    
    /** How long till hit notify fades out completely. */
    float HitNotifyDisplayTime;
    
    /** When we last time hit the enemy. */
    float LastEnemyHitTime;
    
    /** How long till enemy hit notify fades out completely. */
    float LastEnemyHitDisplayTime;
    
    /** Icons for hit indicator. */
    UPROPERTY()
    FCanvasIcon HitNotifyIcon[8];
    
    /** kills background icon. */
    UPROPERTY()
    FCanvasIcon KillsBg;
    
    /** Match timer and player position background icon. */
    UPROPERTY()
    FCanvasIcon TimePlaceBg;
    
    /** Primary weapon background icon. */
    UPROPERTY()
    FCanvasIcon PrimaryWeapBg;
    
    /** Secondary weapon background icon */
    UPROPERTY()
    FCanvasIcon SecondaryWeapBg;
    
    /** Crosshair icons (left, top, right, bottom and center). */
    UPROPERTY()
    FCanvasIcon Crosshair[5];
    
    /** On crosshair indicator that we hit someone. */
    UPROPERTY()
    FCanvasIcon HitNotifyCrosshair;
    
    /** Death messages background icon. */
    UPROPERTY()
    FCanvasIcon DeathMessagesBg;
    
    /** Health bar background icon. */
    UPROPERTY()
    FCanvasIcon HealthBarBg;
    
    /** Health bar icon. */
    UPROPERTY()
    FCanvasIcon HealthBar;
    
    /** Health icon on the health bar. */
    UPROPERTY()
    FCanvasIcon HealthIcon;
    
    /** Kills icon. */
    UPROPERTY()
    FCanvasIcon KillsIcon;
    
    /** Bigger killed icon. */
    UPROPERTY()
    FCanvasIcon KilledIcon;
    
    /** UI scaling factor for other resolutions than Full HD. */
    float ScaleUI;
    
    /** Current animation pulse value. */
    float PulseValue;
    
    /** FontRenderInfo enabling casting shadow.s */
    FFontRenderInfo ShadowedFont;
    
    /** Big "KILLED [PLAYER]" message text above the crosshair. */
    FText CenteredKillMessage;
    
    /** last time we killed someone. */
    float LastKillTime;
    
    /** How long the message will fade out. */
    float KillFadeOutTime;
    
    /** Offsets to display hit indicator parts. */
    FVector2D Offsets[8];
    
    /** Texture for hit indicator. */
    UPROPERTY()
    UTexture2D* HitNotifyTexture;
    
    /** texture for HUD elements. */
    UPROPERTY()
    UTexture2D* HUDMainTexture;
    
    /** Texture for HUD elements. */
    UPROPERTY()
    UTexture2D* HUDAssets02Texture;
    
    /** Overlay shown when health is low. */
    UPROPERTY()
    UTexture2D* LowHealthOverlayTexture;
    
    /** Large font - used for ammo display etc. */
    UPROPERTY()
    UFont* BigFont;
    
    /** Normal font - used for death messages and such. */
    UPROPERTY()
    UFont* NormalFont;
    
    /** General offset for HUD elements. */
    float Offset;
    
    /** Runtime data for hit indicator. */
    FHitData HitNotifyData[8];
    
    /** Active death messages. */
    TArray<FDeathMessage> DeathMessages;
    
    /** Array of information strings to render (Waiting to respawn etc) */
    TArray<FCanvasTextItem> InfoItems;
    
    /**
     * Converts floating point seconds to MM:SS string.
     *
     * @param TimeSeconds		The time to get a string for.
     */
    FString GetTimeString(float TimeSeconds);
    
    /** Draws weapon HUD. */
    void DrawWeaponHUD();
    
    /** Draw player's health bar. */
    void DrawHealth();
    
    /** Draws weapon crosshair. */
    void DrawCrosshair();
    
    /** Draws hit indicator. */
    void DrawHitIndicator();
    
    /** Draw death messages. */
    void DrawDeathMessages();
    
    /** Temporary helper for drawing text-in-a-box. */
    void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);
    
    /** helper for getting uv coords in normalized top,left, bottom, right format */
    void MakeUV(FCanvasIcon& Icon, FVector2D& UV0, FVector2D& UV1, uint16 U, uint16 V, uint16 UL, uint16 VL);
    
    /*
     * Create the chat widget if it doesn't already exist.
     *
     * @return		true if the widget was created.
     */
    bool TryCreateChatWidget();
    
    /*
     * Add information string that will be displayed on the hud. They are added as required and rendered together to prevent overlaps 
     * 
     * @param InInfoString	InInfoString
     */
    void AddMatchInfoString(const FCanvasTextItem InfoItem);
    
    /*
     * Render the info messages.
     *
     * @param YOffset	YOffset from top of canvas to start drawing the messages
     * @param ScaleUI	UI Scale factor
     * @param TextScale	Text scale factor
     *
     * @returns The next Y position to draw any further strings
     */
    float ShowInfoItems(float YOffset, float ScaleUI, float TextScale);
    
};