
#pragma once

#include "GameFramework/Actor.h"
#include "AmethystTypes.h"
#include "AmethystExplosionEffect.generated.h"

//
// Spawnable effect for explosion - NOT replicated to clients
// Each explosion type should be defined as separate blueprint
//
UCLASS(Abstract, Blueprintable)
class AAmethystExplosionEffect : public AActor
{
    GENERATED_UCLASS_BODY()
    
    /** explosion FX */
    UPROPERTY(EditDefaultsOnly, Category=Effect)
    UParticleSystem* ExplosionFX;
    
    /** explosion light */
    UPROPERTY(VisibleDefaultsOnly, Category=Effect)
    TSubobjectPtr<UPointLightComponent> ExplosionLight;
    
    /** how long keep explosion light on? */
    UPROPERTY(EditDefaultsOnly, Category=Effect)
    float ExplosionLightFadeOut;
    
    /** explosion sound */
    UPROPERTY(EditDefaultsOnly, Category=Effect)
    USoundCue* ExplosionSound;
    
    /** explosion decals */
    UPROPERTY(EditDefaultsOnly, Category=Effect)
    struct FDecalData Decal;
    
    /** surface data for spawning */
    UPROPERTY(BlueprintReadOnly, Category=Surface)
    FHitResult SurfaceHit;
    
    /** spawn explosion */
    virtual void BeginPlay() override;
    
    /** update fading light */
    virtual void Tick(float DeltaSeconds) override;
    
private:
    
    /** Point light component name */
    FName ExplosionLightComponentName;
};

