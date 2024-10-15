#include "WorldGameMode.h"

#include "AuraHandler.h"
#include "RoomGenerationData.h"
#include "LevelGenerator.h"
#include "Kismet/GameplayStatics.h"

void AWorldGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	_levelGenerator = GetWorld()->SpawnActor<ALevelGenerator>(ALevelGenerator::StaticClass(), { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	_levelGenerator->GenerateLevelList(_roomGenDataAsset);

	AActor* findAuraHandler = UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass());
	if (findAuraHandler)
	{
		_auraHandler = Cast<AAuraHandler>(findAuraHandler);
		_auraHandler->FetchAllAurasAttached();
	}
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("WARNING: No AuraHandler detected in scene!"));
	}
}
