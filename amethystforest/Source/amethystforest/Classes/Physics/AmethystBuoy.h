// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "OceanManager.h"
#include "amethystforest.h"
#include "AmethystBuoy.generated.h"

UCLASS(Abstract, Blueprintable)
class AMETHYSTFOREST_API AAmethystBuoy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAmethystBuoy();

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

	/** Get objects transform */
	FTransform GetTransform();

	/** Get objects location */
	FVector GetLocation();

	/** Get array of test points */
	TArray<FVector> GetTestPoints() const;

	/** Get mass of object */
	float GetMass() const;

	/** Get Thickness of points */
	float GetThickness() const;

	/** Get mass of object */
	float GetDisplacementRatio() const;


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
	void ProcessWaveHeightatPoint(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointMass, float PointThickness, float DispRatio, float Time);

	/** Determine if the point is under the surface */
	bool IsUnder(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time);

	float ForceMagnitude(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time);

	float ChangeInHeight(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time);

	float WaveHeight(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time);

	FVector WaveHeightIntersection(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time);


	void DisplayTestPoints(FVector Location);

	void ApplyForce(float PointMass, float Magnitude, float DispRatio, FVector Location);

	float MassofPoint(float ObjectMass, float NumPoints);
};
