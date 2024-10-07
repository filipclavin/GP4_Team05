// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChaosBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class GP4_TEAM05_API UChaosBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void UpdateBar (float chaosLevel, float maxChaos);
	UFUNCTION(BlueprintCallable) void TickBar	(float deltaTime);
	UFUNCTION(BlueprintCallable) void SetUp		();

	
protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget)) UProgressBar* _chaosBar;
	UPROPERTY(EditAnywhere) FLinearColor			  _chaosBarFullColor;

	FLinearColor _OrdinaryColor;
	
	
	float _maxChaos = 1000;
	float _previousChaos = 0;
	float _targetChaos = 0;
	float _currentChaos = 0;
	
	float _timeSinceUpdate = 0;

	float easeInOutQuint(float x);

	void ChaosBarFull ();
	void ChaosBarReset();

	bool _chaosBarFull = false;
};
