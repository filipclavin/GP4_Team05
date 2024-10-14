
#include "RoomAnchor.h"
#include "Components/BoxComponent.h"

ARoomAnchor::ARoomAnchor()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* scene = CreateDefaultSubobject<USceneComponent>("Default Scene Component");
}

void ARoomAnchor::BeginPlay()
{
	Super::BeginPlay();
}

void ARoomAnchor::OpenAnchorDoor()
{
	OnAnchorDoorOpenAndClose(true);
}

void ARoomAnchor::CloseAnchorDoor()
{
	OnAnchorDoorOpenAndClose(false);
}

