

#include "amethystforest.h"
#include "AmethystMainMenu.h"
#include "Classes/Player/AmethystPersistentUser.h"
#include "Private/UI/Style/AmethystStyle.h"
#include "Private/UI/Style/AmethystMenuSoundsWidgetStyle.h"
//#include "AmethystLoadingScreen.h"
#include "Slate.h"
#include "GenericPlatformChunkInstall.h"

#define LOCTEXT_NAMESPACE "AmethystGame.HUD.Menu"

#define MAX_BOT_COUNT 8

static const FString MapNames[] = { TEXT("Sanctuary"), TEXT("Highrise") };
static const FName PackageNames[] = { TEXT("Sanctuary.umap"), TEXT("Highrise.umap") };
static const int DefaultTDMMap = 1;
static const int DefaultFFAMap = 0;

//use an EMap index, get back the ChunkIndex that map should be part of.
//Instead of this mapping we should really use the AssetRegistry to query for chunk mappings, but maps aren't members of the AssetRegistry yet.
#if PLATFORM_XBOXONE
static const int ChunkMapping[] = { 1, 1 };
#else
static const int ChunkMapping[] = { 1, 2 };
#endif

#if (PLATFORM_PS4) || (PLATFORM_XBOXONE)
#	define QUICKMATCH_SUPPORTED 0
#else
#	define QUICKMATCH_SUPPORTED 1
#endif

FAmethystMainMenu::~FAmethystMainMenu()
{
    
}

void FAmethystMainMenu::Construct(ULocalPlayer* _PlayerOwner)
{
    bShowingDownloadPct = false;
    
    PlayerOwner = _PlayerOwner;
    
    // read user settings
    UAmethystGameUserSettings* const UserSettings = CastChecked<UAmethystGameUserSettings>(GEngine->GetGameUserSettings());
    
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    BotsCountOpt = 1;
    if(PersistentUser)
    {
        BotsCountOpt = PersistentUser->GetBotsCount();
    }
    
    // number entries 0 up to MAX_BOX_COUNT
    TArray<FText> BotsCountList;
    for (int32 i = 0; i <= MAX_BOT_COUNT; i++)
    {
        BotsCountList.Add(FText::AsNumber(i));
    }
    
    TArray<FText> MapList;
    for (int32 i = 0; i < ARRAY_COUNT(MapNames); ++i)
    {
        MapList.Add(FText::FromString(MapNames[i]));
    }
    
    TArray<FText> OnOffList;
    OnOffList.Add( LOCTEXT("Off","OFF") );
    OnOffList.Add( LOCTEXT("On","ON") );
    
    AmethystOptions = MakeShareable(new FAmethystOptions());
    AmethystOptions->Construct(PlayerOwner);
    AmethystOptions->TellInputAboutKeybindings();
    AmethystOptions->OnApplyChanges.BindSP(this, &FAmethystMainMenu::CloseSubMenu);
    
    //Now that we are here, build our menu
    MenuWidget.Reset();
    MenuWidgetContainer.Reset();
    
    if (GEngine && GEngine->GameViewport)
    {
        SAssignNew(MenuWidget, SAmethystMenuWidget)
        .Cursor(EMouseCursor::Default)
        .PlayerOwner(TWeakObjectPtr<ULocalPlayer>(PlayerOwner))
        .IsGameMenu(false);
        
        
        SAssignNew(MenuWidgetContainer, SWeakWidget)
        .PossiblyNullContent(MenuWidget);
        
        TSharedPtr<FAmethystMenuItem> RootMenuItem;
        
#if Amethyst_CONSOLE_UI
        TSharedPtr<FAmethystMenuItem> MenuItem;
        
#if QUICKMATCH_SUPPORTED
        // QUICK MATCH menu option
        MenuHelper::AddMenuItemSP(RootMenuItem, LOCTEXT("QuickMatch", "QUICK MATCH"), this, &FAmethystMainMenu::OnQuickMatchSelected);
#endif
        
        // HOST menu option
        MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Host", "HOST"));
        
        // submenu under "host"
        MenuHelper::AddMenuItemSP(MenuItem, LOCTEXT("TDM", "TEAM DEATHMATCH"), this, &FAmethystMainMenu::OnSplitScreenSelected);
        
        TSharedPtr<FAmethystMenuItem> NumberOfBotsOption = MenuHelper::AddMenuOptionSP(MenuItem, LOCTEXT("NumberOfBots", "NUMBER OF BOTS"), BotsCountList, this, &FAmethystMainMenu::BotCountOptionChanged);
        NumberOfBotsOption->SelectedMultiChoice = BotsCountOpt;
        
        MapOption = MenuHelper::AddMenuOption(MenuItem, LOCTEXT("SELECTED_LEVEL", "Map"), MapList);
        
        HostLANItem = MenuHelper::AddMenuOptionSP(MenuItem, LOCTEXT("LanMatch", "LAN"), OnOffList, this, &FAmethystMainMenu::LanMatchChanged);
        HostLANItem->SelectedMultiChoice = bIsLanMatch;
        
        // JOIN menu option
        MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Join", "JOIN"));
        
        // submenu under "join"
        MenuHelper::AddMenuItemSP(MenuItem, LOCTEXT("Server", "SERVER"), this, &FAmethystMainMenu::OnJoinServer);
        JoinLANItem = MenuHelper::AddMenuOptionSP(MenuItem, LOCTEXT("LanMatch", "LAN"), OnOffList, this, &FAmethystMainMenu::LanMatchChanged);
        JoinLANItem->SelectedMultiChoice = bIsLanMatch;
        
        // Server list widget that will be called up if appropriate
        MenuHelper::AddCustomMenuItem(JoinServerItem,SAssignNew(ServerListWidget,SAmethystServerList).OwnerWidget(MenuWidget).PlayerOwner(PlayerOwner));
#else
        TSharedPtr<FAmethystMenuItem> MenuItem;
        // HOST menu option
        MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Host", "HOST"));
        
        TSharedPtr<FAmethystMenuItem> NumberOfBotsOption = MenuHelper::AddMenuOptionSP(MenuItem, LOCTEXT("NumberOfBots", "NUMBER OF BOTS"), BotsCountList, this, &FAmethystMainMenu::BotCountOptionChanged);
        NumberOfBotsOption->SelectedMultiChoice = BotsCountOpt;
        
        MapOption = MenuHelper::AddMenuOption(MenuItem, LOCTEXT("SELECTED_LEVEL", "Map"), MapList);
        
        // JOIN menu option
        MenuItem = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Join", "JOIN"));
        
#endif
        
        // Options
        MenuHelper::AddExistingMenuItem(RootMenuItem, AmethystOptions->OptionsItem.ToSharedRef());
        
        if(FSlateApplication::Get().SupportsSystemHelp())
        {
            TSharedPtr<FAmethystMenuItem> HelpSubMenu = MenuHelper::AddMenuItem(RootMenuItem, LOCTEXT("Help", "HELP"));
            HelpSubMenu->OnConfirmMenuItem.BindStatic([](){ FSlateApplication::Get().ShowSystemHelp(); });
        }
        
        // QUIT option (for PC)
#if !Amethyst_CONSOLE_UI
        MenuHelper::AddMenuItemSP(RootMenuItem, LOCTEXT("Quit", "QUIT"), this, &FAmethystMainMenu::OnUIQuit);
#endif
        
        MenuWidget->CurrentMenuTitle = LOCTEXT("MainMenu","MAIN MENU");
        MenuWidget->OnGoBack.BindSP(this, &FAmethystMainMenu::OnMenuGoBack);
        MenuWidget->MainMenu = MenuWidget->CurrentMenu = RootMenuItem->SubMenu;
        MenuWidget->OnMenuHidden.BindSP(this, &FAmethystMainMenu::OnMenuHidden);
        
        AmethystOptions->UpdateOptions();
        MenuWidget->BuildAndShowMenu();
    }
    UpdateMenuOwner();
}

void FAmethystMainMenu::AddMenuToGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        GVC->AddViewportWidgetContent(MenuWidgetContainer.ToSharedRef());
    }
}

void FAmethystMainMenu::RemoveMenuFromGameViewport()
{
    if (GEngine && GEngine->GameViewport)
    {
        UGameViewportClient* const GVC = GEngine->GameViewport;
        GVC->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());
    }
}

void FAmethystMainMenu::Tick(float DeltaSeconds)
{
    IPlatformChunkInstall* ChunkInstaller = FPlatformMisc::GetPlatformChunkInstall();
    if (ChunkInstaller)
    {
        EMap SelectedMap = GetSelectedMap();
        // use assetregistry when maps are added to it.
        int32 MapChunk = ChunkMapping[(int)SelectedMap];
        EChunkLocation::Type ChunkLocation = ChunkInstaller->GetChunkLocation(MapChunk);
        
        FText UpdatedText;
        bool bUpdateText = false;
        if (ChunkLocation == EChunkLocation::NotAvailable)
        {
            float PercentComplete = FMath::Min(ChunkInstaller->GetChunkProgress(MapChunk, EChunkProgressReportingType::PercentageComplete), 100.0f);
            UpdatedText = FText::FromString(FString::Printf(TEXT("%s %4.0f%%"),*LOCTEXT("SELECTED_LEVEL", "Map").ToString(), PercentComplete));
            bUpdateText = true;
            bShowingDownloadPct = true;
        }
        else if (bShowingDownloadPct)
        {
            UpdatedText = LOCTEXT("SELECTED_LEVEL", "Map");
            bUpdateText = true;
            bShowingDownloadPct = false;
        }
        
        if (bUpdateText)
        {
            MapOption->SetText(UpdatedText);
        }
    }
}

bool FAmethystMainMenu::IsTickable() const
{
    return true;
}

TStatId FAmethystMainMenu::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(FAmethystMainMenu, STATGROUP_Tickables);
}

bool FAmethystMainMenu::IsTickableWhenPaused() const
{
    return true;
}

void FAmethystMainMenu::OnMenuHidden()
{
#if Amethyst_CONSOLE_UI
    // Menu was hidden from the top-level main menu, on consoles show the welcome screen again.
    UAmethystGameInstance* const GI = Cast<UAmethystGameInstance>(PlayerOwner->GetGameInstance());
    if (GI)
    {
        GI->GotoState(AmethystGameInstanceState::WelcomeScreen);
    }
#else
    RemoveMenuFromGameViewport();
#endif
}

FAmethystMainMenu::EMap FAmethystMainMenu::GetSelectedMap() const
{
    return (EMap)MapOption->SelectedMultiChoice;
}

void FAmethystMainMenu::CloseSubMenu()
{
    MenuWidget->MenuGoBack(true);
}

void FAmethystMainMenu::OnMenuGoBack(MenuPtr Menu)
{
    // if we are going back from options menu
    if (AmethystOptions->OptionsItem->SubMenu == Menu)
    {
        AmethystOptions->RevertChanges();
    }
}

void FAmethystMainMenu::BotCountOptionChanged(TSharedPtr<FAmethystMenuItem> MenuItem, int32 MultiOptionIndex)
{
    BotsCountOpt = MultiOptionIndex;
    
    UAmethystPersistentUser* PersistentUser = GetPersistentUser();
    if(PersistentUser)
    {
        PersistentUser->SetBotsCount(BotsCountOpt);
    }
}

void FAmethystMainMenu::OnUIQuit()
{
    LockAndHideMenu();
    
    const FAmethystMenuSoundsStyle& MenuSounds = FAmethystStyle::Get().GetWidgetStyle<FAmethystMenuSoundsStyle>("DefaultAmethystMenuSoundsStyle");
    UWorld* const World = PlayerOwner ? PlayerOwner->GetWorld() : nullptr;
    FSlateApplication::Get().PlaySound(MenuSounds.ExitGameSound, OwnerUserIndex);
    MenuHelper::PlaySoundAndCall(World, MenuSounds.ExitGameSound, OwnerUserIndex, this, &FAmethystMainMenu::Quit);
}

void FAmethystMainMenu::Quit()
{
    /* TO DO: GameViewportClient
    UGameViewportClient* const Viewport = PlayerOwner->GetGameInstance()->GetGameViewportClient();
    if (Viewport) 
    {
        Viewport->ConsoleCommand("quit");
    }
     */
}

void FAmethystMainMenu::LockAndHideMenu()
{
    MenuWidget->LockControls(true);
    MenuWidget->HideMenu();
}

void FAmethystMainMenu::DisplayLoadingScreen()
{
    /* TO DO: Loading Screen Module
    IAmethystGameLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IAmethystGameLoadingScreenModule>("AmethystGameLoadingScreen");
    if( LoadingScreenModule != NULL )
    {
        LoadingScreenModule->StartInGameLoadingScreen();
    }
     */
}

void FAmethystMainMenu::UpdateMenuOwner()
{
    OwnerUserIndex = PlayerOwner ? PlayerOwner->ControllerId : 0;
}

bool FAmethystMainMenu::IsMapReady() const
{
    bool bReady = true;
    IPlatformChunkInstall* ChunkInstaller = FPlatformMisc::GetPlatformChunkInstall();
    if (ChunkInstaller)
    {
        EMap SelectedMap = GetSelectedMap();
        // should use the AssetRegistry as soon as maps are added to the AssetRegistry
        int32 MapChunk = ChunkMapping[(int)SelectedMap];
        EChunkLocation::Type ChunkLocation = ChunkInstaller->GetChunkLocation(MapChunk);
        if (ChunkLocation == EChunkLocation::NotAvailable)
        {			
            bReady = false;
        }
    }
    return bReady;
}

UAmethystPersistentUser* FAmethystMainMenu::GetPersistentUser() const
{
    UAmethystLocalPlayer* const SLP = Cast<UAmethystLocalPlayer>(PlayerOwner);
    return SLP ? SLP->GetPersistentUser() : nullptr;
}

ULocalPlayer* FAmethystMainMenu::GetOwningPlayer() const
{
    return PlayerOwner;
}

#undef LOCTEXT_NAMESPACE
