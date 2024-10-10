#include "ExplosiveBarrel.h"

#include "GameFramework/PawnMovementComponent.h"

void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	GetMovementComponent()->Deactivate();
}

void AExplosiveBarrel::Despawn()
{
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}
