// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h" 
#include "Room.h"
#include "RoomGenerationData.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"


ALevelGenerator::ALevelGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelGenerator::LoadNewRoom()
{
	if (!_bridgeRoom)
		return;

	_unloadLastRoom = true;
	_unloadDuration = 1.0f;

	bool success = false;
	_currentRoomDepth++;
	FLatentActionInfo info;
	
	FTransform transform;
	transform.SetTranslation({ 0.0f, 0.0f, 10000.0f });

	TSoftObjectPtr<UWorld> levelInstance;
	if (_currentRoomDepth < _numberOfRooms)
	{
		int checkNumber = _currentRoomDepth % _numberOfRooms;
		
		if (_currentRoomDepth % _numberOfRooms == _roomGenDataAsset->_numOfRoomsPerPOI) 
		{
			if(_currentPOIRoom < _roomGenDataAsset->_POIRoomInstances.Num())
			{
				levelInstance = _roomGenDataAsset->_POIRoomInstances[_currentPOIRoom];
				ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), levelInstance, transform, success);
			}
			_currentPOIRoom++;
		}
		else 
		{
			_previousRoom = _selectedRoom;	
			_selectedRoom = FMath::RandRange(0, _roomGenDataAsset->_regularRoomInstances.Num() - 1);
			
			if (_selectedRoom == _previousRoom)
				_selectedRoom = _selectedRoom == _roomGenDataAsset->_regularRoomInstances.Num() - 1 ? 0 : +1;

			levelInstance = _roomGenDataAsset->_regularRoomInstances[_selectedRoom];
			ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), levelInstance, transform, success);
		}
	}
	else 
	{
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), _roomGenDataAsset->_endRoomInstance, transform, success);
	}
}

void ALevelGenerator::SetCurrentRoom(ARoom* newRoom)
{
	if (newRoom == _bridgeRoom || !_bridgeRoom)
		return;

	if (_currentRoomDepth > -1) 
	{
		newRoom->AnchorToRoom(_bridgeRoom->GetUnusedAnchor(), _bridgeRoom);
	}
	_currentRoom = newRoom;
	_currentRoom->_roomDepth = _currentRoomDepth;
}

void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelGenerator::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (_unloadLastRoom && _unloadDuration <= 0.0f)
	{
		_currentRoom->ActivateRoom();

		FLatentActionInfo info;
		const TArray<ULevelStreaming*>& levels = GetWorld()->GetStreamingLevels();
		UGameplayStatics::UnloadStreamLevel(GetWorld(), levels[_unloadIndex]->GetWorldAssetPackageFName(), info, false);
		_removeInstanceFromList = true;
		_unloadIndex++;
		_unloadLastRoom = false;
	}

	// This is for testing purposes, should be remvoed when remove can be completed via objective
	if (_currentRoom->_hasEntered) 
	{
		_currentRoom->_testDuration -= deltaTime;
		if (_currentRoom->_testDuration <= 0.0f)
			_currentRoom->OnRoomComplete();
	}

	_unloadDuration -= _unloadDuration > 0.0f ? deltaTime : 0.0f;
}

void ALevelGenerator::GenerateLevelList(URoomGenerationData* data)
{
	_bridgeRoom = Cast<ARoom>(UGameplayStatics::GetActorOfClass(GetWorld(), ARoom::StaticClass()));
	if(!_bridgeRoom)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, "OBS! No BridgeRoom available, Level Generation will not be active!");
		return;
	}
	_bridgeRoom->SetBridgeRoom();
	_bridgeRoom->_levelGenerator = this;

	if (data->_POIRoomInstances.IsEmpty())
		return;
														
	_numberOfRooms = data->_POIRoomInstances.Num() * (data->_numOfRoomsPerPOI + 1/*Count in the POI Room*/);

	FLatentActionInfo info;
	bool success = false;

	ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), data->_startRoomInstance, FTransform::Identity, success);
	
	const TArray<ULevelStreaming*>& levels = GetWorld()->GetStreamingLevels();
	_unloadIndex = levels.Num() - 1;

	_roomGenDataAsset = data;
}

