
#include "amethystforestPlayerController.h"
#include "AmethystCheatManager.generated.h"

UCLASS(Within=amethystforestPlayerController)
class UAmethystCheatManager : public UCheatManager
{
    GENERATED_UCLASS_BODY()
    
    UFUNCTION(exec)
    void ToggleInfiniteAmmo();
    
    UFUNCTION(exec)
    void ToggleInfiniteClip();
    
    UFUNCTION(exec)
    void Cheat(const FString& Msg);
    
    UFUNCTION(exec)
    void SpawnBot();
};
