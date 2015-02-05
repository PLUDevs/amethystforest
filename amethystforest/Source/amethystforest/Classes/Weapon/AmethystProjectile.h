// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AmethystWeapon_Projectile.h"
#include "AmethystProjectile.generated.h"


UCLASS(Abstract, Blueprintable)
class AAmethystProjectile : public AActor
{
    GENERATED_UCLASS_BODY()
    
    /** initial setup */
    virtual void PostInitializeComponents() override;
    
    /** setup velocity */
    void InitVelocity(FVector& ShootDirection);
    
    /** handle hit */
    UFUNCTION()
    void OnImpact(const FHitResult& HitResult);
    
protected:
    
    /** movement component */
    UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    TSubobjectPtr<UProjectileMovementComponent> MovementComp;
    
    /** collisions */
    UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    TSubobjectPtr<USphereComponent> CollisionComp;
    
    UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    TSubobjectPtr<UParticleSystemComponent> ParticleComp;
    
    /** effects for explosion */
    UPROPERTY(EditDefaultsOnly, Category=Effects)
    TSubclassOf<class AAmethystExplosionEffect> ExplosionTemplate;
    
    /** controller that fired me (cache for damage calculations) */
    TWeakObjectPtr<AController> MyController;
    
    /** projectile data */
    struct FProjectileWeaponData WeaponConfig;
    
    /** did it explode? */
    UPROPERTY(Transient, ReplicatedUsing=OnRep_Exploded)
    bool bExploded;
    
    /** [client] explosion happened */
    UFUNCTION()
    void OnRep_Exploded();
    
    /** trigger explosion */
    void Explode(const FHitResult& Impact);
    
    /** shutdown projectile and prepare for destruction */
    void DisableAndDestroy();
    
    /** update velocity on client */
    virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
};

