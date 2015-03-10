// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
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

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	/** true = use skeletalmesh */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	bool bUseSkeletalMesh;

protected:

	/** Buoy Skeletal Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* BuoySkeletalMesh;

	/** Buoy Static Mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* BuoyStaticMesh;

	/** Single sound when buoy rocks passed 15 degrees */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* RockingSound;

	/** attaches buoy mesh to pawn */
	void AttachMeshToPawn();

	/** detach mesh from pawn */
	void DetachMeshFromPawn();
	
};
