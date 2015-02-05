
#include "amethystforest.h"
#include "Classes/Player/AmethystCheatManager.h"
#include "Classes/Game/amethystforestGameMode.h"

UAmethystCheatManager::UAmethystCheatManager(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
}

void UAmethystCheatManager::ToggleInfiniteAmmo()
{
    AamethystforestPlayerController* MyPC = GetOuterAamethystforestPlayerController();
    
    MyPC->SetInfiniteAmmo(!MyPC->HasInfiniteAmmo());
    MyPC->ClientMessage(FString::Printf(TEXT("Infinite ammo: %s"), MyPC->HasInfiniteAmmo() ? TEXT("ENABLED") : TEXT("off")));
}

void UAmethystCheatManager::ToggleInfiniteClip()
{
    AamethystforestPlayerController* MyPC = GetOuterAamethystforestPlayerController();
    
    MyPC->SetInfiniteClip(!MyPC->HasInfiniteClip());
    MyPC->ClientMessage(FString::Printf(TEXT("Infinite clip: %s"), MyPC->HasInfiniteClip() ? TEXT("ENABLED") : TEXT("off")));
}

void UAmethystCheatManager::Cheat(const FString& Msg)
{
    GetOuterAamethystforestPlayerController()->ServerCheat(Msg.Left(128));
}

void UAmethystCheatManager::SpawnBot()
{
    AamethystforestPlayerController* const MyPC = GetOuterAamethystforestPlayerController();
    APawn* const MyPawn = MyPC->GetPawn();
    AamethystforestGameMode* const MyGame = MyPC->GetWorld()->GetAuthGameMode<AamethystforestGameMode>();
    if (MyPawn && MyGame)
    {
        FRotator Rot = MyPawn->GetActorRotation();
        FVector Loc = MyPawn->GetActorLocation();
        Loc += Rot.Vector().SafeNormal() * 500;
        
        MyGame->SpawnBot(Loc, Rot);
    }
}