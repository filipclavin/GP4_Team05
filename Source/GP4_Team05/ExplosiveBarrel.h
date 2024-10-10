#pragma once
#include "AuraCharacter.h"
#include "ExplosiveBarrel.generated.h"

UCLASS()
class AExplosiveBarrel : public AAuraCharacter
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	void Despawn();
};
