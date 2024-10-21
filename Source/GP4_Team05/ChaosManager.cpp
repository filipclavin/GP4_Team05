// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosManager.h"

// Sets default values for this component's properties
UChaosManager::UChaosManager()
{
	PrimaryComponentTick.bCanEverTick = true;
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
	_chaosBar->SetCurrentChaos(_currentChaos);
	_chaosBar->ChaosBarReset();
}

void UChaosManager::DisableChaosBar(bool disable)
{
	_chaosBar->DisableChaosBar(disable);
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

	if (_chaosBar == nullptr)
		return;
	
	_chaosBar->UpdateBar(_currentChaos, _maxChaos);
	if (ChaosBarIsFilled())
		_chaosBar->ChaosBarFull();

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

void UChaosManager::enemyKilled(float chaosAmount)
{
	_currentChaos += chaosAmount * _chaosGainScaling;
}

void UChaosManager::addChaos(int chaosToAdd)
{
	_currentChaos += chaosToAdd * _chaosGainScaling;
}
