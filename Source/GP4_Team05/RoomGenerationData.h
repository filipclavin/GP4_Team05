// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomGenerationData.generated.h"

class UWorld;

UCLASS()
class GP4_TEAM05_API URoomGenerationData : public UDataAsset
{
	GENERATED_BODY()
public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly) bool _endlessRun = false;

	// How many regular rooms that will spawn before each POI room
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int _numOfRoomsPerPOI = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UWorld>		  _startRoomInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UWorld>		  _endRoomInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<TSoftObjectPtr<UWorld>> _regularRoomInstances;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<TSoftObjectPtr<UWorld>> _POIRoomInstances;

 };
