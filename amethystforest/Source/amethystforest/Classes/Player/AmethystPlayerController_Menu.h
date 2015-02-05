
#include "AmethystTypes.h"
#include "AmethystPlayerController_Menu.generated.h"

UCLASS()
class AAmethystPlayerController_Menu : public APlayerController
{
    GENERATED_UCLASS_BODY()
    
    /** After game is initialized */
    virtual void PostInitializeComponents() override;
};
