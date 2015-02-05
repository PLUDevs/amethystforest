// Fill out your copyright notice in the Description page of Project Settings.

#include "AmethystTypes.generated.h"
#pragma once

namespace EAmethystMatchState
{
    enum Type
    {
        Warmup,
        Playing,
        Won,
        Lost,
    };
}

namespace EAmethystCrosshairDirection
{
    enum Type
    {
        Left=0,
        Right=1,
        Top=2,
        Bottom=3,
        Center=4
    };
}

namespace EAmethystHudPosition
{
    enum Type
    {
        Left=0,
        FrontLeft=1,
        Front=2,
        FrontRight=3,
        Right=4,
        BackRight=5,
        Back=6,
        BackLeft=7,
    };
}

/** keep in sync with AmethystImpactEffect */
UENUM()
namespace EAmethystPhysMaterialType
{
    enum Type
    {
        Unknown,
        Concrete,
        Dirt,
        Water,
        Metal,
        Wood,
        Grass,
        Glass,
        Flesh,
    };
}

#define Amethyst_SURFACE_Default		SurfaceType_Default
#define Amethyst_SURFACE_Concrete       SurfaceType1
#define Amethyst_SURFACE_Dirt           SurfaceType2
#define Amethyst_SURFACE_Water          SurfaceType3
#define Amethyst_SURFACE_Metal          SurfaceType4
#define Amethyst_SURFACE_Wood           SurfaceType5
#define Amethyst_SURFACE_Grass          SurfaceType6
#define Amethyst_SURFACE_Glass          SurfaceType7
#define Amethyst_SURFACE_Flesh          SurfaceType8

USTRUCT()
struct FDecalData
{
    GENERATED_USTRUCT_BODY()
    
    /** material */
    UPROPERTY(EditDefaultsOnly, Category=Decal)
    UMaterial* DecalMaterial;
    
    /** quad size (width & height) */
    UPROPERTY(EditDefaultsOnly, Category=Decal)
    float DecalSize;
    
    /** lifespan */
    UPROPERTY(EditDefaultsOnly, Category=Decal)
    float LifeSpan;
    
    /** defaults */
    FDecalData()
    : DecalSize(256.f)
    , LifeSpan(10.f)
    {
    }
};

/** replicated information on a hit we've taken */
USTRUCT()
struct FTakeHitInfo
{
    GENERATED_USTRUCT_BODY()
    
    /** The amount of damage actually applied */
    UPROPERTY()
    float ActualDamage;
    
    /** The damage type we were hit with. */
    UPROPERTY()
    UClass* DamageTypeClass;
    
    /** Who hit us */
    UPROPERTY()
    TWeakObjectPtr<class AAmethystCharacter> PawnInstigator;
    
    /** Who actually caused the damage */
    UPROPERTY()
    TWeakObjectPtr<class AActor> DamageCauser;
    
    /** Specifies which DamageEvent below describes the damage received. */
    UPROPERTY()
    int32 DamageEventClassID;
    
    /** Rather this was a kill */
    UPROPERTY()
    uint32 bKilled:1;
    
private:
    
    /** A rolling counter used to ensure the struct is dirty and will replicate. */
    UPROPERTY()
    uint8 EnsureReplicationByte;
    
    /** Describes general damage. */
    UPROPERTY()
    FDamageEvent GeneralDamageEvent;
    
    /** Describes point damage, if that is what was received. */
    UPROPERTY()
    FPointDamageEvent PointDamageEvent;
    
    /** Describes radial damage, if that is what was received. */
    UPROPERTY()
    FRadialDamageEvent RadialDamageEvent;
    
public:
    FTakeHitInfo()
    : ActualDamage(0)
    , DamageTypeClass(NULL)
    , PawnInstigator(NULL)
    , DamageCauser(NULL)
    , DamageEventClassID(0)
    , bKilled(false)
    , EnsureReplicationByte(0)
    {}
    
    FDamageEvent& GetDamageEvent()
    {
        switch (DamageEventClassID)
        {
            case FPointDamageEvent::ClassID:
                if (PointDamageEvent.DamageTypeClass == NULL)
                {
                    PointDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
                }
                return PointDamageEvent;
                
            case FRadialDamageEvent::ClassID:
                if (RadialDamageEvent.DamageTypeClass == NULL)
                {
                    RadialDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
                }
                return RadialDamageEvent;
                
            default:
                if (GeneralDamageEvent.DamageTypeClass == NULL)
                {
                    GeneralDamageEvent.DamageTypeClass = DamageTypeClass ? DamageTypeClass : UDamageType::StaticClass();
                }
                return GeneralDamageEvent;
        }
    }
    
    void SetDamageEvent(const FDamageEvent& DamageEvent)
    {
        DamageEventClassID = DamageEvent.GetTypeID();
        switch (DamageEventClassID)
        {
            case FPointDamageEvent::ClassID:
                PointDamageEvent = *((FPointDamageEvent const*)(&DamageEvent));
                break;
            case FRadialDamageEvent::ClassID:
                RadialDamageEvent = *((FRadialDamageEvent const*)(&DamageEvent));
                break;
            default:
                GeneralDamageEvent = DamageEvent;
        }
        
        DamageTypeClass = DamageEvent.DamageTypeClass;
    }
    
    void EnsureReplication()
    {
        EnsureReplicationByte++;
    }
};