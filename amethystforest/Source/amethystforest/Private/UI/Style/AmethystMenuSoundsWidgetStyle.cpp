
#include "amethystforest.h"
#include "AmethystMenuSoundsWidgetStyle.h"

FAmethystMenuSoundsStyle::FAmethystMenuSoundsStyle()
{
}

FAmethystMenuSoundsStyle::~FAmethystMenuSoundsStyle()
{
}

const FName FAmethystMenuSoundsStyle::TypeName(TEXT("FAmethystMenuSoundsStyle"));

const FAmethystMenuSoundsStyle& FAmethystMenuSoundsStyle::GetDefault()
{
    static FAmethystMenuSoundsStyle Default;
    return Default;
}

void FAmethystMenuSoundsStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UAmethystMenuSoundsWidgetStyle::UAmethystMenuSoundsWidgetStyle( const class FPostConstructInitializeProperties& PCIP )
: Super(PCIP)
{
    
}
