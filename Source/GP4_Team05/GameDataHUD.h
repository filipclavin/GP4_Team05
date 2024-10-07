// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameDataHUD.generated.h"

class AAuraCharacter;
class UCharacterStats;

UCLASS()
class GP4_TEAM05_API AGameDataHUD : public AHUD
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(BlueprintReadWrite) AAuraCharacter* _playerCharacter = nullptr;
	UCharacterStats*							  _playerStats     = nullptr;
};
