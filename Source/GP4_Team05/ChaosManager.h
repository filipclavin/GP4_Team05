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

	//The amount of chaos for a full bar
	UPROPERTY(EditAnywhere, Category="chaos stats") float _maxChaos		     = 1000;
	//how much the full chaos will get multiplied after the bar gets filled
	UPROPERTY(EditAnywhere, Category="chaos stats") float _chaosScaling	     = 1.2f;
	//how much chaos per second you will gain by collecting blood
	UPROPERTY(EditAnywhere, Category="chaos stats") float _chaosBloodGain    = 10;
	//how much chaos you will gain by killing enemies
	UPROPERTY(EditAnywhere, Category="chaos stats") float _chaosEnemyGain    = 100;
	//time in seconds that the chaos bar will be full
	UPROPERTY(EditAnywhere, Category="chaos stats") float _chaosFullDuration = 4;

	float _currentChaos = 0;
	bool  _chaosFull	= false;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY() UChaosBarWidget* _chaosBar = nullptr;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable) void setupChaosManager(UChaosBarWidget* widget);

	
	UFUNCTION(BlueprintCallable) void bloodPickup();
	UFUNCTION(BlueprintCallable) void enemyKilled();
};
