// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosManager.h"

// Sets default values for this component's properties
UChaosManager::UChaosManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UChaosManager::ScaleChaosBar()
{
	_maxChaos *= _maxChaosBarScaling;
}

void UChaosManager::ScaleChaosGain(float gainIncrease)
{
	_chaosGainScaling += gainIncrease;
}

void UChaosManager::ResetChaosBarProgress()
{
	_currentChaos = 0.0f;
}

float UChaosManager::GetCurrentChaos()
{
	return _currentChaos;
}

bool UChaosManager::ChaosBarIsFilled()
{
	return _currentChaos >= _maxChaos ? true: false;
}


// Called when the game starts
void UChaosManager::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UChaosManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_chaosFull)
	{
		return;
	}
	// ...

	if (_currentChaos >= _maxChaos)
	{
		_chaosFull = true;
		
		FTimerHandle barFullHandle;

		//GetWorld()->GetTimerManager().SetTimer(barFullHandle, FTimerDelegate::CreateLambda([this] (){_chaosFull = false;}), _chaosFullDuration, false);
		
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Chaos bar full, this will do something when we implement it"));
		_chaosBar->UpdateBar(_maxChaos, _maxChaos); // fill the bar until next update
		return;
	}
	if (_chaosBar == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Chaos manager not setup"));
		return;
	}
	_chaosBar->UpdateBar(_currentChaos, _maxChaos);
	
}

void UChaosManager::setupChaosManager(UChaosBarWidget* widget)
{
	_chaosBar = widget;

	if (_chaosBar == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Chaos manager not setup"));
		DestroyComponent();
	}
}

//void UChaosManager::bloodPickup()
//{
//	_currentChaos += GetWorld()->GetTime().GetDeltaWorldTimeSeconds()*_chaosBloodGain;
//}

void UChaosManager::enemyKilled(float chaosAmount)
{
	_currentChaos += chaosAmount * _chaosGainScaling;
}

void UChaosManager::addChaos(int chaosToAdd)
{
	_currentChaos += chaosToAdd * _chaosGainScaling;
}
