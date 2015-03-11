// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/Physics/AmethystBuoy.h"


// Sets default values
AAmethystBuoy::AAmethystBuoy(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	BuoyStaticMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Buoy Skeletal Mesh"));
	BuoyStaticMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	BuoyStaticMesh->bChartDistanceFactor = true;
	BuoyStaticMesh->bReceivesDecals = false;
	BuoyStaticMesh->CastShadow = true;
	BuoyStaticMesh->SetCollisionObjectType(ECC_Pawn);
	BuoyStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BuoyStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = BuoyStaticMesh;

	BuoySkeletalMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Buoy Skeletal Mesh"));
	BuoySkeletalMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::OnlyTickPoseWhenRendered;
	BuoySkeletalMesh->bChartDistanceFactor = true;
	BuoySkeletalMesh->bReceivesDecals = false;
	BuoySkeletalMesh->CastShadow = true;
	BuoySkeletalMesh->SetCollisionObjectType(ECC_Pawn);
	BuoySkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BuoySkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BuoySkeletalMesh->AttachParent = BuoyStaticMesh;

	bUseSkeletalMesh = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

}

// Called when the game starts or when spawned
void AAmethystBuoy::BeginPlay()
{
	Super::BeginPlay();

	InitializeBuoyancy();
	
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

void AAmethystBuoy::InitializeBuoyancy()
{
	TArray<FVector> Points = GetTestPoints();
	int32 NumPoints = Points::Num();
	float PointMass = MassofPoint(GetMass(), NumPoints);

	// Loop through the number of points in array
	for (int32 n = 0; n<NumPoints; n++)
	{
		ProcessWaveHeightatPoint(Points[n], Fluid, GetTransform(), PointMass, GetThickness(), GetDisplacementRatio(), GetWorldTime());
	}

}

void AAmethystBuoy::ProcessWaveHeightatPoint(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointMass, float PointThickness, float DispRatio, float Time)
{
	FVector TransformLocation = TransformLocation(ActorTransform, Location);
	bool Under = EvaluateWaveHeight(Location, Liquid, ActorTransform, PointThickness, Time);


}

bool AAmethystBuoy::IsUnder(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time)
{

}

float AAmethystBuoy::ForceMagnitude(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time)
{

}

float AAmethystBuoy::ChangeInHeight(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time)
{

}

float AAmethystBuoy::WaveHeight(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time)
{

}

FVector AAmethystBuoy::WaveHeightIntersection(FVector Location, AOceanManager Liquid, FTransform ActorTransform, float PointThickness, float Time)
{
	
}

void AAmethystBuoy::DisplayTestPoints()
{

}

void AAmethystBuoy::ApplyForce(float PointMass, float Magnitude, float DispRatio, FVector Location)
{

}

void AAmethystBuoy::AttachMeshToPawn()
{

	if(bUseSkeletalMesh)
	{
		BuoyStaticMesh->SetHiddenInGame(true);
		BuoySkeletalMesh->SetHiddenInGame(false);
	}
	else
	{
		BuoyStaticMesh->SetHiddenInGame(false);
		BuoySkeletalMesh->SetHiddenInGame(true);
	}
}

float AAmethystBuoy::MassofPoint(float ObjectMass, float NumPoints)
{
	return ObjectMass/NumPoints;
}

FTransform AAmethystBuoy::GetTransform()
{
	return this->GetActorTransform();
}

FVector AAmethystBuoy::GetLocation()
{
	return this->GetActorLocation();
}

TArray<FVector> AAmethystBuoy::GetTestPoints()
{
	return TestPoints;
}

float AAmethystBuoy::GetMass()
{
	return Mass;
}

float AAmethystBuoy::GetThickness()
{
	return Thickness;
}

float AAmethystBuoy::GetDisplacementRatio()
{
	return DisplacementRatio;
}

