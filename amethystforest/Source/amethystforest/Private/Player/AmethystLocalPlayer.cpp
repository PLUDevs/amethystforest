
#include "amethystforest.h"
#include "Classes/Player/AmethystLocalPlayer.h"

UAmethystLocalPlayer::UAmethystLocalPlayer(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
}

UAmethystPersistentUser* UAmethystLocalPlayer::GetPersistentUser() const
{
    // if persistent data isn't loaded yet, load it
    if (PersistentUser == nullptr)
    {
        UAmethystLocalPlayer* const MutableThis = const_cast<UAmethystLocalPlayer*>(this);
        // casting away constness to enable caching implementation behavior
        MutableThis->LoadPersistentUser();
    }
    return PersistentUser;
}

void UAmethystLocalPlayer::LoadPersistentUser()
{
    if (PersistentUser == NULL)
    {
        PersistentUser = UAmethystPersistentUser::LoadPersistentUser( GetNickname(), ControllerId );
    }
}

void UAmethystLocalPlayer::SetControllerId(int32 NewControllerId)
{
    ULocalPlayer::SetControllerId(NewControllerId);
    
    // if we changed controllerid / user, then we need to load the appropriate persistent user.
    if (PersistentUser != nullptr && ( ControllerId != PersistentUser->GetUserIndex() || GetNickname() != PersistentUser->GetName() ) )
    {
        PersistentUser->SaveIfDirty();
        PersistentUser = nullptr;
    }
    
    if (!PersistentUser)
    {
        LoadPersistentUser();
    }
}

