// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomAnchor.generated.h"

UCLASS()
class GP4_TEAM05_API ARoomAnchor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoomAnchor();

protected:
	UPROPERTY(BlueprintReadWrite) bool _isOccupied;
};
