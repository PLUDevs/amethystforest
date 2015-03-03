// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OceanManager.generated.h"

UCLASS()
class AMETHYSTFOREST_API AOceanManager : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:	

	void Initialize();

	/*
	* Render the info messages.
	*
	* @param location	Absolute location of wave
	* @param time		Time input
	*
	* @returns The next Y position to draw any further strings
	*/
	FVector GetWaveHeightValue(FVector location, float time);

	/*
	* Render the info messages.
	*
	* @param medianWavelength	Wave median length of wave cluster
	* @param medianAmplitude	Wave median amplitude of wave cluster
	* @param position			Position of wave cluster
	* @param medianDirection	Direction of cluster's movement
	* @param steepness			Steepness of cluster
	* @param time				Time input, period of wave is 1/time
	*
	* @returns The next Y position to draw any further strings
	*/
	FVector CalculateGerstnerWaveCluster(float medianWavelength, float medianAmplitude, FVector2D position, FVector2D medianDirection, float steepness, float time);

	FVector CalculateGerstnerWave(float wavelength, float amplitude, FVector2D position, FVector2D direction, float angle, float steepness, float time, float phase);

	FColor GetTextureValue(int32 x, int32 y);

protected:

	UPROPERTY(Category = Ocean, EditAnywhere)
	UTexture* Texture;

private:

	TArray<FColor> ColorBuffer;

	float Size;

	FVector Center;
};
