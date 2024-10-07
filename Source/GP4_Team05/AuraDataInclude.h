#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum AuraType : uint8
{
	// Buff indicates that this Aura increases specific stats or effects an AuraCharacter in a positive way
	BUFF,
	// Debuff indicates that this Aura decreases speciic stats or effects the AuraCharacter in a negative way 
	DEBUFF,
	// Effect only runs OnAuraCast and OnAuraAdd and is removed immediatly (Note that OnAuraRemoved is not called)
	EFFECT,
	// Used for arrays only in code, THIS IS NOT AN ACTUALY AURA TYPE!
	AURA_TYPE_COUNT
};

UENUM(BlueprintType)
enum ElementTypes : uint8
{
	PHYSICAL,
	WATER,
	FIRE,
	EARTH,
	AIR,
	// Count is only for coding usages!
	ELEMENT_TYPE_COUNT
};
