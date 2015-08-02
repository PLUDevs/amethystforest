// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Physics/HeightMapReader.h"

void AHeightMapReader::UpdateBuffer()
{
    ColorBuffer.Reset();
    
    if (RenderTarget != NULL)
    {
        
        FTextureRenderTarget2DResource* textureResource = (FTextureRenderTarget2DResource*)RenderTarget->Resource;
        if (textureResource->ReadPixels(ColorBuffer))
        {
            
        }
    }
}


FColor AHeightMapReader::GetRenderTargetValue(float x, float y)
{
    float size = 10000;
    
    if (RenderTarget == NULL || ColorBuffer.Num() == 0)
        return FColor(0);
    
    float width = RenderTarget->GetSurfaceWidth();
    float height = RenderTarget->GetSurfaceHeight();
    
    //Conver coordinates to texture space
    float normalizedX = (x / size) + 0.5f;
    float normalizedY = (y / size) + 0.5f;
    
    int i = (int)(normalizedX * width);
    int j = (int)(normalizedY * height);
    
    if (i < 0) i = 0;
    if (i >= width) i = width - 1;
    if (j < 0) j = 0;
    if (j >= height) j = height - 1;
    
    int index = i + j * width;
    if (index < 0) index = 0;
    if (index >= ColorBuffer.Num()) index = ColorBuffer.Num();
    
    return ColorBuffer[index];
}