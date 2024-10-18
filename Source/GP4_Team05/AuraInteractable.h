// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraInteractable.generated.h"

class AAuraInteractableSelector;
class AAuraHandler;
class ARoom;

UCLASS()
class GP4_TEAM05_API AAuraInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraInteractable();
	void SetSelectorAndAura(AAuraInteractableSelector* selector, FString auraName, FString description, AAuraHandler* auraHandler)
	{
		_auraSelector    = selector;
		_auraName        = auraName;
		_auraDescription = description;
		_auraHandler     = auraHandler;
	}

	UFUNCTION(BlueprintCallable) void OnPickup();
	
	UFUNCTION(BlueprintNativeEvent) void OnInteractableSpawn();
	void OnInteractableSpawn_Implementation(){}

protected:
	virtual void BeginPlay() override;

public:	
	friend class AAuraInteractableSelector;

	UPROPERTY(BlueprintReadOnly) AAuraHandler*			    _auraHandler;
	UPROPERTY(BlueprintReadOnly) AAuraInteractableSelector* _auraSelector;
	UPROPERTY(BlueprintReadOnly) FString					_auraName = "";
	UPROPERTY(BlueprintReadOnly) FString					_auraDescription = "";

	bool _canBePickedUp		   = true;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
