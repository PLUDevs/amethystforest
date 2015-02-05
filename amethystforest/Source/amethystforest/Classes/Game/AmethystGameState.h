#pragma once //TO DO: pragma once should not be needed.

#include "AmethystGameState.generated.h"

UCLASS()
class AAmethystGameState : public AGameState
{
    GENERATED_UCLASS_BODY()
    
public:
    
    void RequestFinishAndExitToMainMenu();
    
};
