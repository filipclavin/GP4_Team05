#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum AuraType : uint8
{
	// Buff indicates that this Aura increases specific stats or effects an AuraCharacter in a positive way
	BUFF,
	// Debuff indicates that this Aura decreases speciic stats or effects the AuraCharacter in a negative way 
	DEBUFF,
	// Effects is at the moment not super well defined, will update when EFFECT has a purpose
	EFFECT,
	// Used for arrays only in code, THIS IS NOT AN ACTUALY AURA TYPE!
	AURA_TYPE_COUNT
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
