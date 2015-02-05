
#include "amethystforest.h"
#include "AmethystOptionsWidgetStyle.h"

FAmethystOptionsStyle::FAmethystOptionsStyle()
{
}

FAmethystOptionsStyle::~FAmethystOptionsStyle()
{
}

const FName FAmethystOptionsStyle::TypeName(TEXT("FAmethystOptionsStyle"));

const FAmethystOptionsStyle& FAmethystOptionsStyle::GetDefault()
{
    static FAmethystOptionsStyle Default;
    return Default;
}

void FAmethystOptionsStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UAmethystOptionsWidgetStyle::UAmethystOptionsWidgetStyle( const class FPostConstructInitializeProperties& PCIP )
: Super(PCIP)
{
    
}
