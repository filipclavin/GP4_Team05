// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomAnchor.generated.h"

class UBoxComponent;

UCLASS()
class GP4_TEAM05_API ARoomAnchor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomAnchor();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable) void OpenAnchorDoor();
	UFUNCTION(BlueprintCallable) void CloseAnchorDoor();

 	// Play sound effects, move a model, etc. 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAnchorDoorOpenAndClose(bool doorOpened);

	void OnAnchorDoorOpenAndClose_Implementation(bool doorOpened) {}


protected:
	UPROPERTY(EditAnywhere)       UBoxComponent* _doorCollider;
  	UPROPERTY(BlueprintReadWrite) bool _isOccupied;
};
