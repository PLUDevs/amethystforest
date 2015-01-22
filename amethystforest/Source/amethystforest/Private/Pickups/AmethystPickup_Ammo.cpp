
#include "amethystforest.h"
#include "Classes/Pickups/AmethystPickup_Ammo.h"

AAmethystPickup_Ammo::AAmethystPickup_Ammo(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    AmmoClips = 2;
}

bool AAmethystPickup_Ammo::IsForWeapon(UClass* WeaponClass)
{
    return WeaponType->IsChildOf(WeaponClass);
}

bool AAmethystPickup_Ammo::CanBePickedUp(class AAmethystCharacter* TestPawn) const
{
    AAmethystWeapon* TestWeapon = (TestPawn ? TestPawn->FindWeapon(WeaponType) : NULL);
    if (bIsActive && TestWeapon)
    {
        return TestWeapon->GetCurrentAmmo() < TestWeapon->GetMaxAmmo();
    }
    
    return false;
}

void AAmethystPickup_Ammo::GivePickupTo(class AAmethystCharacter* Pawn)
{
    AAmethystWeapon* Weapon = (Pawn ? Pawn->FindWeapon(WeaponType) : NULL);
    if (Weapon)
    {
        Weapon->GiveAmmo(AmmoClips * Weapon->GetAmmoPerClip());
    }
}
