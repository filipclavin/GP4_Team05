// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraInteractableSelector.generated.h"

class ARoom;
class AAuraInteractable;
class AAuraCharacter;
class AAuraHandler;
class USceneComponent;

UCLASS()
class GP4_TEAM05_API AAuraInteractableSelector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraInteractableSelector();

	UFUNCTION(BlueprintCallable) void SpawnInteractablesAtLocation(FVector location, FRotator rotation);

	void RemoveInteractables();
	void SetCurrentRoom(ARoom* room);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	ARoom*					 _currentRoom   = nullptr;
	AAuraCharacter*			 _player        = nullptr;
	AAuraHandler*            _auraHandler   = nullptr;
	UPROPERTY(EditAnywhere)TArray<USceneComponent*> _spawnPosition;
	UPROPERTY(EditAnywhere) TSubclassOf<AAuraInteractable> _interactableToSpawn;

	TArray<AAuraInteractable*> _currentInteractables;
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
