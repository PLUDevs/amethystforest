// Fill out your copyright notice in the Description page of Project Settings.

#include "amethystforest.h"
#include "Classes/Physics/AmethystBuoy.h"
#include "Classes/Physics/OceanManager.h"
#include "Math/UnrealMathUtility.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
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
    
    if (bDisplayPoints)
    {
        DisplayTestPoints();
    }

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

void AAmethystBuoy::ProcessWaveHeightatPoint(FVector PointLocation, FTransform ActorTransform, float PointMass, float PointThickness, float DispRatio, float Time)
{
    FVector NewLocation = UKismetMathLibrary::TransformLocation(ActorTransform, PointLocation);
    ApplyForce(PointMass, ForceMagnitude(NewLocation, ActorTransform, PointThickness, Time), DispRatio, NewLocation);
}

bool AAmethystBuoy::IsUnder(FVector PointLocation, FTransform ActorTransform, float PointThickness, float Time)
{	
	float Delta = ChangeInHeight(PointLocation, ActorTransform, Time);

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
    TArray<FVector> Points = GetTestPoints();
    if(DebugColor == EDebugColor::Black)
    {
        FLinearColor LineColor = FColor Black;
    }
    float ArrowSize = 15;
    FColor Red = Red;
    //FLinearColor LineColor = Red;
    float Duration = -1;
    float Radius = 10;
    float Segments = 12;
    
    // Loop through the number of points in array
    for (int32 n = 0; n<Points.Num(); n++)
    {
        // Dray an up arrow and down arrow on the point surrounded by a sphere
        
        FVector LineStart = Points[n];
        FVector LineEndUp = Points[n] + (0,0,25);
        FVector LineEndDown = Points[n] + (0,0,-25);
        if (bDisplayPoints)
        {
            UKismetSystemLibrary::DrawDebugArrow(
                GetWorld(),
                LineStart,
			    LineEndUp,
			    ArrowSize,
			    LineColor,
			    Duration
            );
		    UKismetSystemLibrary::DrawDebugArrow(
                GetWorld(),
			    LineStart,
			    LineEndDown,
			    ArrowSize,
			    LineColor,
			    Duration
            );
            UKismetSystemLibrary::DrawDebugSphere(
                GetWorld(),
			    LineStart, //Center
			    Radius,
			    Segments,
			    LineColor,
			    Duration
            );
        }
    }
}

void AAmethystBuoy::ApplyForce(float PointMass, float Magnitude, float DispRatio, FVector Location)
{
    FVector Force;
	if (IsUnder(Location, GetTransform(), GetThickness(), GetTime()))
	{
        Force.Set(0.f,0.f, 980.f*DispRatio*PointMass);
		
	}
	else 
	{
        Force.Set(0.0f,0.0f, 980*PointMass);
	}
    
    float a = ForceMagnitude(Location, GetTransform(), GetThickness(), GetTime());
    
    /* Note: AddForceAtLocation without a bone parameter will target the root body instead.
     Might need to make adjustment as the root body of the SkeletalMesh is the SkeletalMesh or
     see if its possible to get the proper FName of the rootcomponent
     */
    if(bUseSkeletalMesh)
    {
        BuoySkeletalMesh->AddForceAtLocation(Force * ForceMagnitude(Location, GetTransform(), GetThickness(), GetTime()), Location);
    }
    else
    {
        BuoyStaticMesh->AddForceAtLocation(Force * ForceMagnitude(Location, GetTransform(), GetThickness(), GetTime()), Location);
    }

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
    return this->GetTransform();
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
    UWorld * World = GetWorld();
    return World->GetTimeSeconds();
}

FString GetColorEnumAsString(EDebugColor::Type EnumValue)
{
    const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EDebugColor"), true);
    if(!EnumPtr) return FString("Invalid");
    
    return EnumPtr->GetEnumName(EnumValue);
}

