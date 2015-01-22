
#include "amethystforest.h"
#include "AmethystMenuWidgetStyle.h"

FAmethystMenuStyle::FAmethystMenuStyle()
{
}

FAmethystMenuStyle::~FAmethystMenuStyle()
{
}

const FName FAmethystMenuStyle::TypeName(TEXT("FAmethystMenuStyle"));

const FAmethystMenuStyle& FAmethystMenuStyle::GetDefault()
{
    static FAmethystMenuStyle Default;
    return Default;
}

void FAmethystMenuStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
    OutBrushes.Add(&HeaderBackgroundBrush);
    OutBrushes.Add(&LeftBackgroundBrush);
    OutBrushes.Add(&RightBackgroundBrush);
}


UAmethystMenuWidgetStyle::UAmethystMenuWidgetStyle( const class FPostConstructInitializeProperties& PCIP )
: Super(PCIP)
{
    
}
