#include "AuraInteractable.h"
#include "AuraInteractableSelector.h"
// Sets default values
AAuraInteractable::AAuraInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAuraInteractable::OnPickup()
{
	_auraSelector->RemoveInteractables();
}

// Called when the game starts or when spawned
void AAuraInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAuraInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

