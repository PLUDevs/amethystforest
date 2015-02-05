

#pragma once

#include "GameFramework/GameMode.h"
#include "Bots/AmethystAIController.h"
#include "Bots/AmethystBot.h"
#include "Pickups/AmethystPickup.h"
#include "amethystforestGameMode.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class AMETHYSTFOREST_API AamethystforestGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()
    
    virtual void StartPlay() override;
    
    /** The bot pawn class */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameMode)
    TSubclassOf<class APawn> BotPawnClass;
    
    UFUNCTION(exec)
    void SetAllowBots(bool bInAllowBots, int32 InMaxBots = 8);
    
    /** Initialize the game. This is called before actors' PreInitializeComponents. */
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    
    /** select best spawn point for player */
    virtual AActor* ChoosePlayerStart(AController* Player) override;
    
    /** always pick new random spawn */
    virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
    
    /** returns default pawn class for given controller */
    virtual UClass* GetDefaultPawnClassForController(AController* InController) override;
    
    /** always create cheat manager */
    virtual bool AllowCheats(APlayerController* P) override;
    
    /** spawns default bot */
    class AAmethystBot* SpawnBot(FVector SpawnLocation, FRotator SpawnRotation);

protected:
    
    /** scale for self instigated damage */
    UPROPERTY(config)
    float DamageSelfScale;
    
    UPROPERTY(config)
    int32 MaxBots;
    
    UPROPERTY()
    TArray<AAmethystAIController*> BotControllers;
    
    bool bAllowBots;
    
    /** spawning all bots for this game */
    void SpawnBotsForGame();
    
    /** initialization for bot after spawning */
    virtual void InitBot(AAmethystBot* Bot, int BotNumber);
    
    /** check if player can use spawnpoint */
    virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;
    
    /** check if player should use spawnpoint */
    virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;
    
public:
    
    /** Send Player to Main Menu */
    void RequestFinishAndExitToMainMenu();
    
    /** get the name of the bots count option, can be used in server travel URL if we use online*/
    static FString GetBotsCountOptionName();
    
    UPROPERTY()
    TArray<class AAmethystPickup*> LevelPickups;
	
};
