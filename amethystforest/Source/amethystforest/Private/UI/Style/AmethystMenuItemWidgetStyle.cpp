
#include "amethystforest.h"
#include "AmethystMenuItemWidgetStyle.h"

FAmethystMenuItemStyle::FAmethystMenuItemStyle()
{
}

FAmethystMenuItemStyle::~FAmethystMenuItemStyle()
{
}

const FName FAmethystMenuItemStyle::TypeName(TEXT("FAmethystMenuItemStyle"));

const FAmethystMenuItemStyle& FAmethystMenuItemStyle::GetDefault()
{
    static FAmethystMenuItemStyle Default;
    return Default;
}

void FAmethystMenuItemStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
    OutBrushes.Add(&BackgroundBrush);
    OutBrushes.Add(&LeftArrowImage);
    OutBrushes.Add(&RightArrowImage);
}


UAmethystMenuItemWidgetStyle::UAmethystMenuItemWidgetStyle( const class FPostConstructInitializeProperties& PCIP )
: Super(PCIP)
{
    
}
