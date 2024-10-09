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
	_unloadDuration = 0.1f;

	bool success = false;
	_currentRoomIndex++;
	FLatentActionInfo info;

	TSoftObjectPtr<UWorld> levelInstance;
	if (_currentRoomIndex < _numberOfRooms)
	{
		int checkNumber = _currentRoomIndex % _numberOfRooms;
		
		if (_currentRoomIndex % _numberOfRooms == _roomGenDataAsset->_numOfRoomsPerPOI) 
		{
			if(_currentPOIRoom < _roomGenDataAsset->_POIRoomInstances.Num())
			{
				levelInstance = _roomGenDataAsset->_POIRoomInstances[_currentPOIRoom];
				ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), levelInstance, FTransform::Identity, success);
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
			ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), levelInstance, FTransform::Identity, success);
		}
	}
	else 
	{
		ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), _roomGenDataAsset->_endRoomInstance, FTransform::Identity, success);
	}
}

void ALevelGenerator::SetCurrentRoom(ARoom* newRoom)
{
	if (newRoom == _bridgeRoom || !_bridgeRoom)
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

	if (_removeInstanceFromList) 
	{
		GetWorld()->RemoveStreamingLevelAt(_unloadIndex);
		_removeInstanceFromList = false;
	}

	if (_unloadLastRoom && _unloadDuration <= 0.0f)
	{
		FLatentActionInfo info;
		const TArray<ULevelStreaming*>& levels = GetWorld()->GetStreamingLevels();
		UGameplayStatics::UnloadStreamLevel(GetWorld(), levels[_unloadIndex]->GetWorldAssetPackageFName(), info, false);
		_removeInstanceFromList = true;
		//_unloadIndex++;
		_unloadLastRoom = false;
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

