#include "AuraInteractableSelector.h"
#include "AuraInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "AuraHandler.h"
#include "Room.h"
#include "AuraCharacter.h"
#include "Aura.h"

AAuraInteractableSelector::AAuraInteractableSelector()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(FName("Root Component"));
	SetRootComponent(root);

	for (INT32 i = 0; i < 3; i++) {
		FString intString;
		intString.AppendInt(i);
		_spawnPosition.Add(CreateDefaultSubobject<USceneComponent>(FName("Aura Position "+ intString)));
		_spawnPosition[i]->AttachToComponent(root, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void AAuraInteractableSelector::SpawnInteractablesAtLocation(FVector location, FRotator rotation)
{
	SetActorLocation(location);
	SetActorRotation(rotation);

	_currentInteractables.Empty();
	_currentInteractables.Reserve(3);

	TArray<UAura*> auraList = _auraHandler->GetRandomAuraFromSpawnList();
	for (INT32 i = 0; i < _spawnPosition.Num(); i++)
	{
		AAuraInteractable* auraBuff = GetWorld()->SpawnActor<AAuraInteractable>(_interactableToSpawn, _spawnPosition[i]->GetComponentToWorld());
		_currentInteractables.Add(auraBuff);
		auraBuff->SetSelectorAndAura(this, auraList[i]->AuraName(), _auraHandler->GetAuraDescription(auraList[i]->AuraName()), auraList[i]->GetElementType(), _auraHandler);
		auraBuff->OnInteractableSpawn();
	}
}

void AAuraInteractableSelector::RemoveInteractables()
{
	for (INT32 i = 0; i < _currentInteractables.Num(); i++)
	{
		_currentInteractables[i]->_canBePickedUp = false;
		_currentInteractables[i]->Destroy();
		_currentInteractables[i] = nullptr;
	}

	_currentRoom->OnInteractablePickup();
}

void AAuraInteractableSelector::SetCurrentRoom(ARoom* room)
{
	_currentRoom = room;
}

// Called when the game starts or when spawned
void AAuraInteractableSelector::BeginPlay()
{
	Super::BeginPlay();

	_auraHandler = Cast<AAuraHandler>(UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass()));
	_player      = Cast<AAuraCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AAuraCharacter::StaticClass()));

	if (!_player || !_auraHandler)
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("Couldn't find Player or AuraHandler"));
}

// Called every frame
void AAuraInteractableSelector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

