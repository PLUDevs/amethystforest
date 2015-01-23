

#include "amethystforest.h"
#include "../../Classes/Bots/AmethystAIController.h"
#include "../../Classes/Bots/AmethystBot.h"


AAmethystBot::AAmethystBot(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	AIControllerClass = AAmethystAIController::StaticClass();

	UpdatePawnMeshes();

	AttackRange = 100;
	AttackDamage = 10;
	bUseControllerRotationYaw = true;
}


bool AAmethystBot::IsFirstPerson() const
{
	return false;
}
void AAmethystBot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}

void AAmethystBot::ReceiveHit
(class UPrimitiveComponent * MyComp, class AActor * Other, class UPrimitiveComponent * OtherComp, bool bSelfMoved,
FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{

	AAmethystAIController * AI = Cast<AAmethystAIController>(Super::GetController());



}
float AAmethystBot::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	FRotator Rotation;
	Rotation.Yaw = -90;
	Rotation.Pitch = 0;
	Rotation.Roll = 0;
	Super::AddActorLocalRotation(Rotation, false);
    return 0;
}
void AAmethystBot::PerformMeleeAttack()
{
	const FVector StartTrace = GetActorLocation();
	const FVector ShootDir = GetActorRotation().Vector();
	const FVector EndTrace = StartTrace + ShootDir * AttackRange;



	//  We perform a sphere sweep, checking if there is something in the cylinder that trace creates, and if it finds something, damage it.
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true);
	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->SweepSingle(Hit, StartTrace, EndTrace, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(25), TraceParams);


	if (Hit.Actor != NULL)
	{
		ACharacter *character = Cast<ACharacter>(Hit.GetActor());
		if (character != NULL)
		{
			FPointDamageEvent PointDmg;
			PointDmg.DamageTypeClass = UDamageType::StaticClass();
			PointDmg.HitInfo = Hit;
			PointDmg.ShotDirection = ShootDir;
			PointDmg.Damage = AttackDamage;
			character->TakeDamage(AttackDamage, PointDmg, Controller, this);
		}
	}
}




