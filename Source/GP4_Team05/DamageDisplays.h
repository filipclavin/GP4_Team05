// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageDisplays.generated.h"

class UTextBlock;

UCLASS()
class GP4_TEAM05_API UDamageDisplays : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	TArray<UTextBlock*> _damageTexts;
};
