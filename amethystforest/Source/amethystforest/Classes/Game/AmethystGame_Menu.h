
#include "AmethystGame_Menu.generated.h"

UCLASS()
class AAmethystGame_Menu : public AGameMode
{
    GENERATED_UCLASS_BODY()
    
public:
    
    // Begin AGameMode interface
    /** skip it, menu doesn't require player start or pawn */
    virtual void RestartPlayer(class AController* NewPlayer) override;
    
    /** Returns game session class to use */
//    TO DO: GameSession Class, don't think we need.
//    virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
    // End AGameMode interface
    
protected:
    
    /** Display a loading screen */
    void ShowLoadingScreen();
};
