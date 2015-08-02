// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HeightMapReader.generated.h"

/**
 * 
 */
UCLASS()
class AMETHYSTFOREST_API AHeightMapReader : public AActor
{
	GENERATED_BODY()
	
    UPROPERTY(Category = HeightMap, EditAnywhere)
    UTextureRenderTarget2D* RenderTarget;
    
    UFUNCTION(BlueprintCallable, Category = "HeightMap|Update")
    void UpdateBuffer();
    
    UFUNCTION(BlueprintCallable, Category = "HeightMap|Texture Helper")
    FColor GetRenderTargetValue(float x, float y);
    
private:
    
    TArray<FColor> ColorBuffer;
	
	
	
};