// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

class ARoom;
class URoomGenerationData;
class ULevelStreaming;

UCLASS()
class GP4_TEAM05_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

	// This function is called when entering a bridge collider, or a new room. will see what I decide.
	void LoadNewRoom();

	UFUNCTION(BlueprintCallable) int GetRoomDepth() { return _currentRoomDepth; }

	void SetCurrentRoom(ARoom* newRoom);
	ARoom* GetBridgeRoom() { return _bridgeRoom; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere) int     _maxNumberOfRooms;

	ARoom*				 _currentRoom	   = nullptr;
	ARoom*				 _bridgeRoom	   = nullptr;
	URoomGenerationData* _roomGenDataAsset = nullptr;

private:
	friend class AWorldGameMode;
	
	virtual void Tick(float deltaTime) override;

	void GenerateLevelList(URoomGenerationData* data);
	
	bool  _unloadLastRoom         = false;
	bool  _removeInstanceFromList = false;
	float _unloadDuration         = 1.0f;
	int   _currentRoomDepth       = -1;
	int   _currentPOIRoom         = 0;
	int   _numberOfRooms          = 0;
	int   _unloadIndex            = -1;
	int   _selectedRoom           = -1;
	int   _previousRoom           = -1;
};
