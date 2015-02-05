
#include "amethystforest.h"
#include "Classes/Weapon/AmethystWeapon_Projectile.h"
#include "Classes/Weapon/AmethystProjectile.h"

AAmethystWeapon_Projectile::AAmethystWeapon_Projectile(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AAmethystWeapon_Projectile::FireWeapon()
{
    FVector ShootDir = GetAdjustedAim();
    FVector Origin = GetMuzzleLocation();
    
    // trace from camera to check what's under crosshair
    const float ProjectileAdjustRange = 10000.0f;
    const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
    const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
    FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
    
    // and adjust directions to hit that actor
    if (Impact.bBlockingHit)
    {
        const FVector AdjustedDir = (Impact.ImpactPoint - Origin).SafeNormal();
        bool bWeaponPenetration = false;
        
        const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
        if (DirectionDot < 0.0f)
        {
            // shooting backwards = weapon is penetrating
            bWeaponPenetration = true;
        }
        else if (DirectionDot < 0.5f)
        {
            // check for weapon penetration if angle difference is big enough
            // raycast along weapon mesh to check if there's blocking hit
            
            FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
            FVector MuzzleEndTrace = Origin;
            FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);
            
            if (MuzzleImpact.bBlockingHit)
            {
                bWeaponPenetration = true;
            }
        }
        
        if (bWeaponPenetration)
        {
            // spawn at crosshair position
            Origin = Impact.ImpactPoint - ShootDir * 10.0f;
        }
        else
        {
            // adjust direction to hit
            ShootDir = AdjustedDir;
        }
    }
    
    ServerFireProjectile(Origin, ShootDir);
}

bool AAmethystWeapon_Projectile::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
    return true;
}

void AAmethystWeapon_Projectile::ServerFireProjectile_Implementation(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
    FTransform SpawnTM(ShootDir.Rotation(), Origin);
    AAmethystProjectile* Projectile = Cast<AAmethystProjectile>(UGameplayStatics::BeginSpawningActorFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
    if (Projectile)
    {
        Projectile->Instigator = Instigator;
        Projectile->SetOwner(this);
        Projectile->InitVelocity(ShootDir);
        
        UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
    }
}

void AAmethystWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
    Data = ProjectileConfig;
}



