// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h" 
#include "Room.h"
#include "RoomGenerationData.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"


ALevelGenerator::ALevelGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelGenerator::LoadNewRoom()
{
	_previousRoomInstance = _currentRoomInstance;
	_unloadLastRoom = true;
	_unloadDuration = 0.5f;
	
	_currentRoomIndex++;
	FLatentActionInfo info;
	if (_currentRoomIndex < _numberOfRooms)
	{
		int checkNumber = _currentRoomIndex % _numberOfRooms;

		if (_currentRoomIndex % _numberOfRooms == _roomGenDataAsset->_numOfRoomsPerPOI) 
		{
			if(_currentPOIRoom < _roomGenDataAsset->_POIRoomInstances.Num())
			{
				_currentRoomInstance = _roomGenDataAsset->_POIRoomInstances[_currentPOIRoom];
				UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), _currentRoomInstance, true, false, info);
			}
			_currentPOIRoom++;
		}
		else 
		{
			_previousRoom = _selectedRoom;	
			_selectedRoom = FMath::RandRange(0, _roomGenDataAsset->_regularRoomInstances.Num() - 1);
			
			if (_selectedRoom == _previousRoom)
				_selectedRoom = _selectedRoom == _roomGenDataAsset->_regularRoomInstances.Num() - 1 ? 0 : +1;

			_currentRoomInstance = _roomGenDataAsset->_regularRoomInstances[_selectedRoom];
			UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), _currentRoomInstance, true, false, info);
		}
	}
	else 
	{
		UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), _roomGenDataAsset->_endRoomInstance, true, false, info);
	}
}

void ALevelGenerator::SetCurrentRoom(ARoom* newRoom)
{
	if (newRoom == _bridgeRoom)
		return;

	if (_currentRoomIndex > -1) {
		newRoom->AnchorToRoom(_bridgeRoom->GetUnusedAnchor(), _bridgeRoom);
	}
	_currentRoom = newRoom;
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
		FLatentActionInfo info;
		if(_previousRoomInstance != nullptr)
			UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(GetWorld(), _previousRoomInstance, info, false);
		_unloadLastRoom = false;
	}

	_unloadDuration -= _unloadDuration > 0.0f ? deltaTime : 0.0f;
}

void ALevelGenerator::GenerateLevelList(URoomGenerationData* data)
{
	_bridgeRoom = Cast<ARoom>(UGameplayStatics::GetActorOfClass(GetWorld(), ARoom::StaticClass()));
	_bridgeRoom->SetBridgeRoom();
	_bridgeRoom->_levelGenerator = this;

	if (data->_POIRoomInstances.IsEmpty())
		return;
														
	_numberOfRooms    = data->_POIRoomInstances.Num() * (data->_numOfRoomsPerPOI + 1/*Count in the POI Room*/);
				
	FLatentActionInfo info;
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), data->_startRoomInstance, true, false, info);

	_currentRoomInstance = data->_startRoomInstance;

	_roomGenDataAsset = data;
}

