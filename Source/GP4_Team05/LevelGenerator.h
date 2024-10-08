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

	// This function is called when entering a corridor collider, or a new room. will see what I decide.
	UFUNCTION(BlueprintCallable) void LoadNewRoom();

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
	
	bool  _unloadLastRoom  = false;
	float _unloadDuration = 1.0f;

	int _currentRoomIndex = -1;
	int _currentPOIRoom   = 0;
	int _numberOfRooms    = 0;

	//TSoftObjectPtr<UWorld> _currentRoomInstance;
	//TSoftObjectPtr<UWorld> _previousRoomInstance;

	int _unloadIndex = -1;

	FString _currentRoomInstance;
	FString _previousRoomInstance;

	//int _currentRoomInstance;
	//int _previousRoomInstance;

	int  _selectedRoom = -1;
	int  _previousRoom = -1;
};
