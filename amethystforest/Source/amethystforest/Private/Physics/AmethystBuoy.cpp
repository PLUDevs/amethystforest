// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/Physics/AmethystBuoy.h"


// Sets default values
AAmethystBuoy::AAmethystBuoy(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	BuoySkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Buoy Skeletal Mesh"));
	BuoySkeletalMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	BuoySkeletalMesh->bChartDistanceFactor = true;
	BuoySkeletalMesh->bReceivesDecals = false;
	BuoySkeletalMesh->CastShadow = true;
	BuoySkeletalMesh->SetCollisionObjectType(ECC_Pawn);
	BuoySkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BuoySkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	bUseSkeletalMesh = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;


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

void AAmethystBuoy::AttachMeshToPawn()
{
	DetachMeshFromPawn();

	if(bUseSkeletalMesh)
	{
		
	}
}

void AAmethystBuoy::DetachMeshFromPawn()
{
	BuoySkeletalMesh->DetachFromParent();
	BuoySkeletalMesh->SetHiddenInGame(true);

	BuoyStaticMesh->DetachMeshFromPawn();
	BuoyStaticMesh->SetHiddenInGame(true);
}

