
#pragma once
#include "Slate.h"
#include "Widgets/AmethystMenuItem.h"
#include "Widgets/SAmethystMenuWidget.h"
#include "AmethystOptions.h"


class FAmethystMainMenu : public TSharedFromThis<FAmethystMainMenu>, public FTickableGameObject
{
public:
    
    virtual ~FAmethystMainMenu();
    
    /** build menu */
    void Construct(ULocalPlayer* _PlayerOwner);
    
    /** Add the menu to the gameviewport so it becomes visible */
    void AddMenuToGameViewport();
    
    /** Remove from the gameviewport. */
    void RemoveMenuFromGameViewport();
    
    /** TickableObject Functions */
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickableWhenPaused() const override;
    
    /** Returns the player that owns the main menu. */
    ULocalPlayer* GetOwningPlayer() const;
    
protected:
    
    enum class EMap
    {
        ESancturary,
        EHighRise,
        EMax,
    };
    
    /** Owning player */
    ULocalPlayer* PlayerOwner;
    
    /** Owning User Index */
    int32 OwnerUserIndex;
    
    /** Amethyst options */
    TSharedPtr<class FAmethystOptions> AmethystOptions;
    
    /** menu widget */
    TSharedPtr<class SAmethystMenuWidget> MenuWidget;
    
    /* used for removing the MenuWidget */
    TSharedPtr<class SWeakWidget> MenuWidgetContainer;
    
    /** Map selection widget */
    TSharedPtr<FAmethystMenuItem> MapOption;
    
    /** Track if we are showing a map download pct or not. */
    bool bShowingDownloadPct;
    
    EMap GetSelectedMap() const;
    
    /** goes back in menu structure */
    void CloseSubMenu();
    
    /** called when going back to previous menu */
    void OnMenuGoBack(MenuPtr Menu);
    
    /** called when menu hide animation is finished */
    void OnMenuHidden();
    
    /** bot count option changed callback */
    void BotCountOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex);
    
    /** Plays sound and calls Quit */
    void OnUIQuit();
    
    /** Quits the game */
    void Quit();
    
    /** Lock the controls and hide the main menu */
    void LockAndHideMenu();
    
    /** Display the loading screen. */
    void DisplayLoadingScreen();
    
    /** Updates the OwnerIndex from the player controller */
    void UpdateMenuOwner();
    
    /** Checks the ChunkInstaller to see if the selected map is ready for play */
    bool IsMapReady() const;
    
    /** Get the persistence user associated with PCOwner*/
    UAmethystPersistentUser* GetPersistentUser() const;
    
    /** number of bots in game */
    int32 BotsCountOpt;
    
};
