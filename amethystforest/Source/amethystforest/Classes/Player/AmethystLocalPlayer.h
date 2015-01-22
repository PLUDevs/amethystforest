//TO DO: pragma once prevents from casting persistent user to a localplayer
#pragma once //remove once fix

#include "AmethystPersistentUser.h"
#include "AmethystLocalPlayer.generated.h"

UCLASS(config=Engine, transient)
class UAmethystLocalPlayer : public ULocalPlayer
{
    GENERATED_UCLASS_BODY()
    
public:
    
    virtual void SetControllerId(int32 NewControllerId) override;
    
    class UAmethystPersistentUser* GetPersistentUser() const;
    
    /** Initializes the PersistentUser */
    void LoadPersistentUser();
    
private:
    /** Persistent user data stored between sessions (i.e. the user's savegame) */
    UPROPERTY()
    class UAmethystPersistentUser* PersistentUser;
    
};



