#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldGameMode.generated.h"

class ALevelGenerator;
class AAuraHandler;
class URoomGenerationData;

UCLASS()
class GP4_TEAM05_API AWorldGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	UPROPERTY(EditAnywhere) URoomGenerationData* _roomGenDataAsset = nullptr;

private:
	ALevelGenerator* _levelGenerator = nullptr;
	AAuraHandler*    _auraHandler    = nullptr;
};
