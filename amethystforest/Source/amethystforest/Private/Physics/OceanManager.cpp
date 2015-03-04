// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/Physics/OceanManager.h"


// Sets default values
AOceanManager::AOceanManager(const class FObjectInitializer& PCIP)
: Super(PCIP)
{
	// Set center and size
	Center = FVector(0, 0, 0);
	Size = 10000;

}

void AOceanManager::Initialize()
{
	ColorBuffer.Reset();
}

FVector AOceanManager::GetWaveHeightValue(FVector location, float time)
{
	// Calculate gerstner wave
	FVector sum = FVector(0, 0, 0);

	sum += CalculateGerstnerWaveCluster(2500, 200, FVector2D(location.X, location.Y), FVector2D(0, 1), 0.5f, time);
	sum += CalculateGerstnerWaveCluster(1000, 115, FVector2D(location.X, location.Y), FVector2D(0, 1), 0.5f, time);

	return sum / 2;

}

FVector AOceanManager::CalculateGerstnerWaveCluster(float medianWavelength, float medianAmplitude, FVector2D position, FVector2D medianDirection, float steepness, float time)
{
	// Calculate gerstner wave
	FVector sum = FVector(0, 0, 0);

	sum += CalculateGerstnerWave(medianWavelength, medianAmplitude, position, medianDirection, 0, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 0.5f, medianAmplitude * 0.5f, position, medianDirection, -0.1f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 2.0f, medianAmplitude * 2.0f, position, medianDirection, 0.1f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 1.25f, medianAmplitude * 1.25f, position, medianDirection, 0.05f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 0.75f, medianAmplitude * 0.75f, position, medianDirection, 0.075f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 1.5f, medianAmplitude * 1.5f, position, medianDirection, -0.125f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 0.825f, medianAmplitude * 0.825f, position, medianDirection, 0.063f, steepness, time, 0);
	sum += CalculateGerstnerWave(medianWavelength * 0.65f, medianAmplitude * 0.65f, position, medianDirection, -0.11f, steepness, time, 0);

	return sum / 8;
}

FVector AOceanManager::CalculateGerstnerWave(float wavelength, float amplitude, FVector2D position, FVector2D direction, float angle, float steepness, float time, float phase)
{
	float lambda = (2 * PI) / wavelength;

	FVector dir = FVector(direction.X, direction.Y, 0);
	dir = dir.RotateAngleAxis(angle * 360, FVector(0, 0, 1));

	FVector2D rotatedDirection = FVector2D(dir.X, dir.Y);

	float wavePhase = lambda * FVector2D::DotProduct(rotatedDirection, position) + (time + phase);

	float c = FMath::Cos(wavePhase);
	float s = FMath::Sin(wavePhase);

	float QA = steepness * amplitude;

	return FVector(QA * rotatedDirection.X * c, QA * rotatedDirection.Y * c, amplitude * s);
}

FColor AOceanManager::GetTextureValue(int32 x, int32 y)
{
	if (Texture == NULL)
		return FColor();

	float width = Texture->GetSurfaceWidth();
	float height = Texture->GetSurfaceHeight();
	/* TO DO fix this:
	// Get a pointer to the beginning of the mipmap memory location and lock for reading and writing
	uint8* mipMap = (uint8*)Texture->GetRunningPlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY);

	FColor* data = (FColor*)&mipMap[(x + (int)width * y) * sizeof(FColor)];

	Texture->GetRunningPlatformData->Mips[0].BulkData.Unlock();

	return *data;
	*/
	return FColor(); //Remove later
}
