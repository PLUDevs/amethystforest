// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "AmethystBuoy.h"


// Sets default values
AAmethystBuoy::AAmethystBuoy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAmethystBuoy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAmethystBuoy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AAmethystBuoy::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

