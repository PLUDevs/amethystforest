
#include "AmethystPickup_Ammo.generated.h"

// A pickup object that replenishes ammunition for a weapon
UCLASS(Abstract, Blueprintable)
class AAmethystPickup_Ammo : public AAmethystPickup
{
    GENERATED_UCLASS_BODY()
    
    /** check if pawn can use this pickup */
    virtual bool CanBePickedUp(class AAmethystCharacter* TestPawn) const override;
    
    bool IsForWeapon(UClass* WeaponClass);
    
protected:
    
    /** how much ammo does it give? */
    UPROPERTY(EditDefaultsOnly, Category=Pickup)
    int32 AmmoClips;
    
    /** which weapon gets ammo? */
    UPROPERTY(EditDefaultsOnly, Category=Pickup)
    TSubclassOf<class AAmethystWeapon> WeaponType;
    
    /** give pickup */
    virtual void GivePickupTo(class AAmethystCharacter* Pawn) override;
};
