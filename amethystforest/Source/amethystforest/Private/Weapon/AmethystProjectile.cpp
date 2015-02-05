
#include "amethystforest.h"
#include "Classes/Weapon/AmethystProjectile.h"
#include "Classes/Effects/AmethystExplosionEffect.h"
#include "Particles/ParticleSystemComponent.h"

AAmethystProjectile::AAmethystProjectile(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->AlwaysLoadOnClient = true;
    CollisionComp->AlwaysLoadOnServer = true;
    CollisionComp->bTraceComplexOnMove = true;
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
    CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    RootComponent = CollisionComp;
    
    ParticleComp = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
    ParticleComp->bAutoActivate = false;
    ParticleComp->bAutoDestroy = false;
    ParticleComp->AttachParent = RootComponent;
    
    MovementComp = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    MovementComp->UpdatedComponent = CollisionComp;
    MovementComp->InitialSpeed = 2000.0f;
    MovementComp->MaxSpeed = 2000.0f;
    MovementComp->bRotationFollowsVelocity = true;
    MovementComp->ProjectileGravityScale = 0.f;
    
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
    bReplicateInstigator = true;
    bReplicateMovement = true;
}

void AAmethystProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    MovementComp->OnProjectileStop.AddDynamic(this, &AAmethystProjectile::OnImpact);
    CollisionComp->MoveIgnoreActors.Add(Instigator);
    
    AAmethystWeapon_Projectile* OwnerWeapon = Cast<AAmethystWeapon_Projectile>(GetOwner());
    if (OwnerWeapon)
    {
        OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
    }
    
    SetLifeSpan( WeaponConfig.ProjectileLife );
    MyController = GetInstigatorController();
}

void AAmethystProjectile::InitVelocity(FVector& ShootDirection)
{
    if (MovementComp)
    {
        MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
    }
}

void AAmethystProjectile::OnImpact(const FHitResult& HitResult)
{
    if (Role == ROLE_Authority && !bExploded)
    {
        Explode(HitResult);
        DisableAndDestroy();
    }
}

void AAmethystProjectile::Explode(const FHitResult& Impact)
{
    if (ParticleComp)
    {
        ParticleComp->Deactivate();
    }
    
    // effects and damage origin shouldn't be placed inside mesh at impact point
    const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
    
    if (WeaponConfig.ExplosionDamage > 0 && WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType)
    {
        UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
    }
    
    if (ExplosionTemplate)
    {
        const FRotator SpawnRotation = Impact.ImpactNormal.Rotation();
        
        AAmethystExplosionEffect* EffectActor = GetWorld()->SpawnActorDeferred<AAmethystExplosionEffect>(ExplosionTemplate, NudgedImpactLocation, SpawnRotation);
        if (EffectActor)
        {
            EffectActor->SurfaceHit = Impact;
            UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(SpawnRotation, NudgedImpactLocation));
        }
    }
    
    bExploded = true;
}

void AAmethystProjectile::DisableAndDestroy()
{
    UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
    if (ProjAudioComp && ProjAudioComp->IsPlaying())
    {
        ProjAudioComp->FadeOut(0.1f, 0.f);
    }
    
    MovementComp->StopMovementImmediately();
    
    // give clients some time to show explosion
    SetLifeSpan( 2.0f );
}

void AAmethystProjectile::OnRep_Exploded()
{
    FVector ProjDirection = GetActorRotation().Vector();
    
    const FVector StartTrace = GetActorLocation() - ProjDirection * 200;
    const FVector EndTrace = GetActorLocation() + ProjDirection * 150;
    FHitResult Impact;
    
    if (!GetWorld()->LineTraceSingle(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(TEXT("ProjClient"), true, Instigator)))
    {
        // failsafe
        Impact.ImpactPoint = GetActorLocation();
        Impact.ImpactNormal = -ProjDirection;
    }
    
    Explode(Impact);
}

void AAmethystProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
    if (MovementComp)
    {
        MovementComp->Velocity = NewVelocity;
    }
}

void AAmethystProjectile::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps( OutLifetimeProps );
    
    DOREPLIFETIME( AAmethystProjectile, bExploded );
}


