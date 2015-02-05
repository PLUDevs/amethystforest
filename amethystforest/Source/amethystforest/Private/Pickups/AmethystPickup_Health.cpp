
#include "amethystforest.h"
#include "Classes/Pickups/AmethystPickup_Health.h"

AAmethystPickup_Health::AAmethystPickup_Health(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    Health = 50;
}

bool AAmethystPickup_Health::CanBePickedUp(class AAmethystCharacter* TestPawn) const
{
    return TestPawn && (TestPawn->Health < TestPawn->GetMaxHealth());
}

void AAmethystPickup_Health::GivePickupTo(class AAmethystCharacter* Pawn)
{
    if (Pawn)
    {
        Pawn->Health = FMath::Min(FMath::TruncToInt(Pawn->Health) + Health, Pawn->GetMaxHealth());
    }
}
