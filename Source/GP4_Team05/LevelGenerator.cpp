// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h" 
#include "Room.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"


ALevelGenerator::ALevelGenerator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelGenerator::LoadNewRoom()
{
	_currentRoomIndex++;
	if (_currentRoomIndex < _rooms.Num())
	{
		if (_currentRoomIndex > 0) {
			_unloadLastRoom = true;
			_unloadDuration = 0.5f;
		}

		FLatentActionInfo info;
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName(_rooms[_currentRoomIndex]), true, false, info);
	}
	else 
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, GetName() + "End of Room list");
	}
}

void ALevelGenerator::SetCurrentRoom(ARoom* newRoom)
{
	if (newRoom == _bridgeRoom)
		return;

	if (_currentRoomIndex > 0) {
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

	if (_unloadLastRoom && _unloadDuration <= 0.0f && _currentRoomIndex > 0)
	{
		FLatentActionInfo info;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(_rooms[_currentRoomIndex - 1]), info, false);
		_unloadLastRoom = false;
	}

	_unloadDuration -= _unloadDuration > 0.0f ? deltaTime : 0.0f;
}

void ALevelGenerator::GenerateLevelList()
{
	_bridgeRoom = Cast<ARoom>(UGameplayStatics::GetActorOfClass(GetWorld(), ARoom::StaticClass()));
	_bridgeRoom->SetBridgeRoom();
	_bridgeRoom->_levelGenerator = this;

	TArray<FString> regularRooms;
	TArray<FString> narrativeRooms;

	FString startRoom;
	FString endRoom;

	// Fetch the names of each sublevel and assign them to the correct string/list
	const TArray<ULevelStreaming*>& streamedLevels = GetWorld()->GetStreamingLevels();
	for (short i = 0; i < streamedLevels.Num(); i++)
	{
		FString levelName = streamedLevels[i]->GetWorldAssetPackageName();
		if (levelName.Contains("Bridge"))
			continue;
		
		if (levelName.Contains("Start"))
		{
			startRoom = levelName;
			continue;
		}

		if (levelName.Contains("End"))
		{
			endRoom = levelName;
			continue;
		}

		if (levelName.Contains("POI"))
			narrativeRooms.Add(levelName);

		else
			regularRooms.Add(levelName);
	}

	_rooms.Add(startRoom);
	
	if (!narrativeRooms.IsEmpty()) 
	{
		int numberOfRegularRooms = _maxNumberOfRooms - narrativeRooms.Num();
		numberOfRegularRooms /= narrativeRooms.Num();
	
		for (short i = 0; i < narrativeRooms.Num(); i++)
		{
			for (short j = 0; j < regularRooms.Num(); j++)
			{
				INT32 randomRoom = FMath::RandRange(0, regularRooms.Num() - 1);
				_rooms.Add(regularRooms[j]);
			}
			_rooms.Add(narrativeRooms[i]);
		}
	}

	_rooms.Add(endRoom);

	LoadNewRoom();
}

