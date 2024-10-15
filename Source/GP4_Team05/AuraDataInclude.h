#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum AuraType : uint8
{
	// Buff indicates that this Aura increases specific stats or effects an AuraCharacter in a positive way
	BUFF,
	// Debuff indicates that this Aura decreases speciic stats or effects the AuraCharacter in a negative way 
	DEBUFF,
	// This Aura is a buff applied to the AuraCharacter that is only called when it hits something. The functions that get called are on AuraCast and OnAuraMeleeHit. NOTE that this aura does not call OnAuraAdd, OnAuraRemoved, OnAuraTick or OnAuraUpdate!
	ON_HIT,
	// Used for arrays only in code, THIS IS NOT AN ACTUALY AURA TYPE!
	AURA_TYPE_COUNT
};

UENUM(BlueprintType)
enum AuraAttackType
{
	// Calls auras that are activated by melee attacks
	MELEE,
	// Calls auras that are activated by Fireball Attack
	FIRE_ATTACK,
	// Calls auras that are activated by Lightning Attack
	LIGHTNING_ATTACK,
	// Calls auras that are activated bu Dash
	DASH
};

UENUM(BlueprintType)
enum ElementTypes : uint8
{
	PHYSICAL,
	FIRE,
	LIGHTNING,
	WATER,
	EARTH,
	// Count is only for coding usages!
	ELEMENT_TYPE_COUNT
};
