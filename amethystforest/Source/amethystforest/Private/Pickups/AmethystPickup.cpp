
#include "amethystforest.h"
#include "Classes/Pickups/AmethystPickup.h"
#include "Particles/ParticleSystemComponent.h"
#include "Classes/Game/amethystforestGameMode.h"

AAmethystPickup::AAmethystPickup(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
    TSubobjectPtr<UCapsuleComponent> CollisionComp = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CollisionComp"));
    CollisionComp->InitCapsuleSize(40.0f, 50.0f);
    CollisionComp->SetCollisionObjectType(COLLISION_PICKUP);
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    RootComponent = CollisionComp;
    
    PickupPSC = PCIP.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("PickupFX"));
    PickupPSC->bAutoActivate = false;
    PickupPSC->bAutoDestroy = false;
    PickupPSC->AttachParent = RootComponent;
    
    RespawnTime = 10.0f;
    bIsActive = false;
    PickedUpBy = NULL;
    
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
}

void AAmethystPickup::BeginPlay()
{
    Super::BeginPlay();
    
    RespawnPickup();
    
    // register on pickup list (server only), don't care about unregistering (in FinishDestroy) - no streaming
    AamethystforestGameMode* GameMode = GetWorld()->GetAuthGameMode<AamethystforestGameMode>();
    if (GameMode)
    {
        GameMode->LevelPickups.Add(this);
    }
}

void AAmethystPickup::ReceiveActorBeginOverlap(class AActor* Other)
{
    Super::ReceiveActorBeginOverlap(Other);
    PickupOnTouch(Cast<AAmethystCharacter>(Other));
}

bool AAmethystPickup::CanBePickedUp(class AAmethystCharacter* TestPawn) const
{
    return TestPawn && TestPawn->IsAlive();
}

void AAmethystPickup::GivePickupTo(class AAmethystCharacter* Pawn)
{
}

void AAmethystPickup::PickupOnTouch(class AAmethystCharacter* Pawn)
{
    if (bIsActive && Pawn && Pawn->IsAlive() && !IsPendingKill())
    {
        if (CanBePickedUp(Pawn))
        {
            GivePickupTo(Pawn);
            PickedUpBy = Pawn;
            
            if (!IsPendingKill())
            {
                bIsActive = false;
                OnPickedUp();
                
                if (RespawnTime > 0.0f)
                {
                    GetWorldTimerManager().SetTimer(this, &AAmethystPickup::RespawnPickup, RespawnTime, false);
                }
            }
        }
    }
}

void AAmethystPickup::RespawnPickup()
{
    bIsActive = true;
    PickedUpBy = NULL;
    OnRespawned();
    
    TArray<AActor*> OverlappingPawns;
    GetOverlappingActors(OverlappingPawns, AAmethystCharacter::StaticClass());
    
    for (int32 i = 0; i < OverlappingPawns.Num(); i++)
    {
        PickupOnTouch(Cast<AAmethystCharacter>(OverlappingPawns[i]));
    }
}

void AAmethystPickup::OnPickedUp()
{
    if (RespawningFX)
    {
        PickupPSC->SetTemplate(RespawningFX);
        PickupPSC->ActivateSystem();
    }
    else
    {
        PickupPSC->DeactivateSystem();
    }
    
    if (PickupSound && PickedUpBy)
    {
        UGameplayStatics::PlaySoundAttached(PickupSound, PickedUpBy->GetRootComponent());
    }
    
    OnPickedUpEvent();
}

void AAmethystPickup::OnRespawned()
{
    if (ActiveFX)
    {
        PickupPSC->SetTemplate(ActiveFX);
        PickupPSC->ActivateSystem();
    }
    else
    {
        PickupPSC->DeactivateSystem();
    }
    
    const bool bJustSpawned = CreationTime <= (GetWorld()->GetTimeSeconds() + 5.0f);
    if (RespawnSound && !bJustSpawned)
    {
        UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
    }
    
    OnRespawnEvent();
}

void AAmethystPickup::OnRep_IsActive()
{
    if (bIsActive)
    {
        OnRespawned();
    }
    else
    {
        OnPickedUp();
    }
}

void AAmethystPickup::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps( OutLifetimeProps );
    
    DOREPLIFETIME( AAmethystPickup, bIsActive );
    DOREPLIFETIME( AAmethystPickup, PickedUpBy );
}