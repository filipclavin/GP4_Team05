#include "WorldGameMode.h"

#include "AuraHandler.h"
#include "RoomGenerationData.h"
#include "LevelGenerator.h"

void AWorldGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	_levelGenerator = GetWorld()->SpawnActor<ALevelGenerator>(ALevelGenerator::StaticClass(), { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	_levelGenerator->GenerateLevelList(_roomGenDataAsset);

}
