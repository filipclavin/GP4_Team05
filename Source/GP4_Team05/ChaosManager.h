// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChaosBarWidget.h"
#include "Components/ActorComponent.h"
#include "ChaosManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP4_TEAM05_API UChaosManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChaosManager();

	void ScaleChaosBar();
	void ScaleChaosGain(float gainIncreas);

	float _currentChaos = 0;
	bool  _chaosFull	= false;
protected:

	//The amount of chaos for a full bar
	UPROPERTY(EditAnywhere, Category="chaos stats") float _maxChaos		   = 1000;
	//How much the chaos bar scales, only scales per rooms that uses the chaos bar.
	UPROPERTY(EditAnywhere, Category="chaos stats") float _maxChaosBarScaling = 1.2f;
	//how much chaos per second you will gain by collecting blood. Can move this to an aura instead.
	UPROPERTY(EditAnywhere, Category="chaos stats") float _chaosGainScaling	= 1.0f;
		
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY() UChaosBarWidget* _chaosBar = nullptr;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable) void setupChaosManager(UChaosBarWidget* widget);

	
	//UFUNCTION(BlueprintCallable) void bloodPickup();
	UFUNCTION(BlueprintCallable) void enemyKilled(float chaosAmount);
	//intended for miscellaneous sources of chaos 
	UFUNCTION(BlueprintCallable) void addChaos	 (int chaosToAdd);
};
