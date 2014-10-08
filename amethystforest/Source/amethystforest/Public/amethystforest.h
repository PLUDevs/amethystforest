

#pragma once

#include "Engine.h"
#include "../Classes/Player/AmethystCharacter.h"
#include "../Classes//Weapon/AmethystWeapon.h"
#include "ParticleDefinitions.h"
#include "ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "SoundDefinitions.h"
#include "Net/UnrealNetwork.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1
#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#define COLLISION_PIKCUP		ECC_GameTraceChannel