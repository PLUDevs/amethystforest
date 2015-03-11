// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/Physics/AmethystBuoy.h"
#include "Classes/Physics/OceanManager.h"
#include "Math/UnrealMathUtility.h"
#include "Components/PrimitiveComponent.h"
#include "Math/Color.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAmethystBuoy::AAmethystBuoy(const class FObjectInitializer& PCIP) : Super(PCIP)
{
	BuoyStaticMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Buoy Skeletal Mesh"));
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
	int32 NumPoints = Points.Num();
	float PointMass = MassofPoint(GetMass(), NumPoints);

	// Loop through the number of points in array
	for (int32 n = 0; n<NumPoints; n++)
	{
		ProcessWaveHeightatPoint(Points[n], GetTransform(), PointMass, GetThickness(), GetDisplacementRatio(), GetTime());
	}

}

void AAmethystBuoy::ProcessWaveHeightatPoint(FVector Location, FTransform ActorTransform, float PointMass, float PointThickness, float DispRatio, float Time)
{
    FVector NewLocation = UKismetMathLibrary::TransformLocation(ActorTransform, Location);
	bool Under = IsUnder(Location, ActorTransform, PointThickness, Time);


}

bool AAmethystBuoy::IsUnder(FVector Location, FTransform ActorTransform, float PointThickness, float Time)
{	
	float Delta = ChangeInHeight(Location, ActorTransform, Time);

	if (Delta > 0){return true;}

	return false;
}

float AAmethystBuoy::ForceMagnitude(FVector Location, FTransform ActorTransform, float PointThickness, float Time)
{
	float HeightDifference = ChangeInHeight(Location, ActorTransform, Time);
	float Magnitude = pow(HeightDifference/PointThickness,3);
    if (Magnitude < -1)
    {
        Magnitude = -1;
    }
    else if(Magnitude > 1)
    {
        Magnitude = 1;
    }

	return fabs(Magnitude);
}

float AAmethystBuoy::ChangeInHeight(FVector Location, FTransform ActorTransform, float Time)
{
    FVector NewLocation = UKismetMathLibrary::TransformLocation(ActorTransform, Location);
	FVector HeightValue = WaveHeightValue(NewLocation, Time);

	float HeightDifference =(HeightValue.Z - NewLocation.Z);

	return HeightDifference;
	
}

FVector AAmethystBuoy::WaveHeightIntersection(FVector Location, FTransform ActorTransform, float Time)
{
    FVector NewLocation = UKismetMathLibrary::TransformLocation(ActorTransform, Location);
	FVector HeightValue = WaveHeightValue(NewLocation, Time);
	FVector Intersection = NewLocation + HeightValue;
	Intersection.Z = NewLocation.Z;

	return Intersection;
}

FVector AAmethystBuoy::WaveHeightValue(FVector Location, float Time)
{
	return Fluid->GetWaveHeightValue(Location, Time);
}

void AAmethystBuoy::DisplayTestPoints()
{
	FVector LineStart = GetLocation();
	FVector LineEndUp = GetLocation() + (0,0,25);
	FVector LineEndDown = GetLocation() + (0,0,-25);
	float ArrowSize = 15;
    FColor C = (255.f,0.f,0.f,0.f);
	FLinearColor LineColor = C;
	float Duration = -1;
	float Radius = 10;
	float Segments = 12;
	if (bDisplayPoints)
	{
		DrawDebugArrow(
			LineStart,
			LineEndUp,
			ArrowSize,
			LineColor,
			Duration
        );
		DrawDebugArrow(
			LineStart,
			LineEndDown,
			ArrowSize,
			LineColor,
			Duration
        );
		DrawDebugSphere(
			LineStart, //Center
			Radius,
			Segments,
			LineColor,
			Duration
        );
	}
}

void AAmethystBuoy::ApplyForce(float PointMass, float Magnitude, float DispRatio, FVector Location)
{
	if (IsUnder(Location, GetTransform(), GetThickness(), GetTime()))
	{
        FVector Force;
        Force.Set(0.f,0.f, 980*PointMass);
		
	}
	else 
	{
        FVector Force;
        Force.Set(0.0f,0.0f, 980*PointMass);
	}

    this->UPrimitiveComponent::AddForce(Location, Force * ForceMagnitude(Location, GetActorTransform(), GetThickness(), GetWorldTime()));

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
    return this->Actor::GetActorTransform();
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

float AAmethystBuoy::GetTime()
{
    return GetWorldTime();
}

