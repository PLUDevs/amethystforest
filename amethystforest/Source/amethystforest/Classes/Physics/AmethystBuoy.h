// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "OceanManager.h"
#include "amethystforest.h"
#include "AmethystBuoy.generated.h"

UCLASS(Abstract, Blueprintable)
class AMETHYSTFOREST_API AAmethystBuoy : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** true = use skeletalmesh */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	bool bUseSkeletalMesh;

	/** true = display test points, for debugging purposes */
	UPROPERTY(EditDefaultsOnly, Category = Debugging)
	bool bDisplayPoints;
    
    /** Name of the bone to apply force to */
    UPROPERTY(EditDefaultsOnly, Category = Mesh)
    FName BoneName;

	/** Get objects transform */
	FTransform GetTransform();

	/** Get objects location */
	FVector GetLocation();

	/** Get array of test points */
	TArray<FVector> GetTestPoints();

	/** Get mass of object */
	float GetMass();

	/** Get Thickness of points */
	float GetThickness();

	/** Get mass of object */
	float GetDisplacementRatio();


protected:

	/* Liquid object will be displaced in */
	UPROPERTY(Transient)
	class AOceanManager* Fluid;

	/** Buoy Skeletal Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* BuoySkeletalMesh;

	/** Buoy Static Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* BuoyStaticMesh;

	/** Array of test points */
	UPROPERTY(EditDefaultsOnly, Category = Points)
	TArray<FVector> TestPoints;

	/** Mass of object */
	UPROPERTY(EditDefaultsOnly, Category = Physics)
	float Mass;

	/* Thickness of each point */
	UPROPERTY(EditDefaultsOnly, Category = Physics)
	float Thickness;

	/** Object's Displacement Ratio */
	UPROPERTY(EditDefaultsOnly, Category = Physics)
	float DisplacementRatio;

	/** Single sound when buoy rocks passed 15 degrees */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* RockingSound;

	/** attaches buoy mesh to pawn */
	void AttachMeshToPawn();

private:
	/** Starts the rending bouancy */
	void InitializeBuoyancy(); 

	/** Process a test point to determine what buoyant forces are to be applied */
	void ProcessWaveHeightatPoint(FVector Location, FTransform ActorTransform, float PointMass, float PointThickness, float DispRatio, float Time);

	/** Determine if the point is under the surface */
	bool IsUnder(FVector Location, FTransform ActorTransform, float PointThickness, float Time);

	/** Determine the magnitude of the force exerted onto the object, this relates to the tickness of the point and wave height*/
	float ForceMagnitude(FVector Location, FTransform ActorTransform, float PointThickness, float Time);

	/** Calculate the change in height */
	float ChangeInHeight(FVector Location, FTransform ActorTransform, float Time);

	/** Find the point where the wave intersects */
	FVector WaveHeightIntersection(FVector Location, FTransform ActorTransform, float Time);

	/** For Debugging: Draws out test points */
	void DisplayTestPoints();

	/** Apply appropriate force onto object */
	void ApplyForce(float PointMass, float Magnitude, float DispRatio, FVector Location);

	/** Calculate Mass of each point */
	float MassofPoint(float ObjectMass, float NumPoints);
    
    /** Get in game time */
    float GetTime();

	/** Calculate the wave height value from the OceanManager class*/
	FVector WaveHeightValue(FVector Location, float Time);
};
