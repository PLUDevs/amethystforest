
#include "amethystforest.h"
#include "Classes/Player/AmethystPersistentUser.h"

UAmethystPersistentUser::UAmethystPersistentUser(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
    SetToDefaults();
}

void UAmethystPersistentUser::SetToDefaults()
{
    bIsDirty = false;
    
    bInvertedYAxis = false;
    AimSensitivity = 1.0f;
    Gamma = 2.2f;
    BotsCount = 1;
}

bool UAmethystPersistentUser::IsAimSensitivityDirty() const
{
    bool bIsDirty = false;
    
    // Fixme: UAmethystPersistentUser is not setup to work with multiple worlds.
    // For now, user settings are global to all world instances.
    if (GEngine)
    {
        TArray<APlayerController*> PlayerList;
        GEngine->GetAllLocalPlayerControllers(PlayerList);
        
        for (auto It = PlayerList.CreateIterator(); It; ++It)
        {
            APlayerController* PC = *It;
            if (!PC || !PC->Player || !PC->PlayerInput)
            {
                continue;
            }
            
            // Update key bindings for the current user only
            ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
            if(!LocalPlayer || LocalPlayer->ControllerId != UserIndex)
            {
                continue;
            }
            
            // check if the aim sensitivity is off anywhere
            for (int32 Idx = 0; Idx < PC->PlayerInput->AxisMappings.Num(); Idx++)
            {
                FInputAxisKeyMapping &AxisMapping = PC->PlayerInput->AxisMappings[Idx];
                if (AxisMapping.AxisName == "Lookup" || AxisMapping.AxisName == "LookupRate" || AxisMapping.AxisName == "Turn" || AxisMapping.AxisName == "TurnRate")
                {
                    if (FMath::Abs(AxisMapping.Scale) != GetAimSensitivity())
                    {
                        bIsDirty = true;
                        break;
                    }
                }
            }
        }
    }
    
    return bIsDirty;
}

bool UAmethystPersistentUser::IsInvertedYAxisDirty() const
{
    bool bIsDirty = false;
    if (GEngine)
    {
        TArray<APlayerController*> PlayerList;
        GEngine->GetAllLocalPlayerControllers(PlayerList);
        
        for (auto It = PlayerList.CreateIterator(); It; ++It)
        {
            APlayerController* PC = *It;
            if (!PC || !PC->Player || !PC->PlayerInput)
            {
                continue;
            }
            
            // Update key bindings for the current user only
            ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
            if(!LocalPlayer || LocalPlayer->ControllerId != UserIndex)
            {
                continue;
            }
            
            bIsDirty |= PC->PlayerInput->GetInvertAxis("Lookup") != GetInvertedYAxis();
            bIsDirty |= PC->PlayerInput->GetInvertAxis("LookupRate") != GetInvertedYAxis();
        }
    }
    
    return bIsDirty;
}

void UAmethystPersistentUser::SavePersistentUser()
{
    UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
    bIsDirty = false;
}

UAmethystPersistentUser* UAmethystPersistentUser::LoadPersistentUser(FString SlotName, const int32 UserIndex)
{
    UAmethystPersistentUser* Result = nullptr;
    
    // first set of player signins can happen before the UWorld exists, which means no OSS, which means no user names, which means no slotnames.
    // Persistent users aren't valid in this state.
    if (SlotName.Len() > 0)
    {
        Result = Cast<UAmethystPersistentUser>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
        if (Result == NULL)
        {
            // if failed to load, create a new one
            Result = Cast<UAmethystPersistentUser>( UGameplayStatics::CreateSaveGameObject(UAmethystPersistentUser::StaticClass()) );
        }
        check(Result != NULL);
        
        Result->SlotName = SlotName;
        Result->UserIndex = UserIndex;
    }
    
    return Result;
}

void UAmethystPersistentUser::SaveIfDirty()
{
    if (bIsDirty || IsInvertedYAxisDirty() || IsAimSensitivityDirty())
    {
        SavePersistentUser();
    }
}

void UAmethystPersistentUser::AddMatchResult(int32 MatchKills, int32 MatchDeaths, int32 MatchBulletsFired, int32 MatchRocketsFired, bool bIsMatchWinner)
{
    Kills += MatchKills;
    Deaths += MatchDeaths;
    BulletsFired += MatchBulletsFired;
    RocketsFired += MatchRocketsFired;
    
    if (bIsMatchWinner)
    {
        Wins++;
    }
    else
    {
        Losses++;
    }
    
    bIsDirty = true;
}

void UAmethystPersistentUser::TellInputAboutKeybindings()
{
    TArray<APlayerController*> PlayerList;
    GEngine->GetAllLocalPlayerControllers(PlayerList);
    
    for (auto It = PlayerList.CreateIterator(); It; ++It)
    {
        APlayerController* PC = *It;
        if (!PC || !PC->Player || !PC->PlayerInput)
        {
            continue;
        }
        
        // Update key bindings for the current user only
        ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->Player);
        if(!LocalPlayer || LocalPlayer->ControllerId != UserIndex)
        {
            continue;
        }
        
        //set the aim sensitivity
        for (int32 Idx = 0; Idx < PC->PlayerInput->AxisMappings.Num(); Idx++)
        {
            FInputAxisKeyMapping &AxisMapping = PC->PlayerInput->AxisMappings[Idx];
            if (AxisMapping.AxisName == "Lookup" || AxisMapping.AxisName == "LookupRate" || AxisMapping.AxisName == "Turn" || AxisMapping.AxisName == "TurnRate")
            {
                AxisMapping.Scale = (AxisMapping.Scale < 0.0f) ? -GetAimSensitivity() : +GetAimSensitivity();
            }
        }
        PC->PlayerInput->ForceRebuildingKeyMaps();
        
        //invert it, and if does not equal our bool, invert it again
        if (PC->PlayerInput->GetInvertAxis("LookupRate") != GetInvertedYAxis())
        {
            PC->PlayerInput->InvertAxis("LookupRate");
        }
        
        if (PC->PlayerInput->GetInvertAxis("Lookup") != GetInvertedYAxis())
        {
            PC->PlayerInput->InvertAxis("Lookup");
        }
    }
}

int32 UAmethystPersistentUser::GetUserIndex() const
{
    return UserIndex;
}

void UAmethystPersistentUser::SetInvertedYAxis(bool bInvert)
{
    bIsDirty |= bInvertedYAxis != bInvert;
    
    bInvertedYAxis = bInvert;
}
void UAmethystPersistentUser::SetAimSensitivity(float InSensitivity)
{
    bIsDirty |= AimSensitivity != InSensitivity;
    
    AimSensitivity = InSensitivity;
}
void UAmethystPersistentUser::SetGamma(float InGamma)
{
    bIsDirty |= Gamma != InGamma;
    
    Gamma = InGamma;
}
void UAmethystPersistentUser::SetBotsCount(int32 InCount)
{
    bIsDirty = BotsCount != InCount;
    
    BotsCount = InCount;
}
