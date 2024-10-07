// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosBarWidget.h"

#include "FrameTypes.h"
#include "Components/ProgressBar.h"

void UChaosBarWidget::UpdateBar(float chaosLevel, float maxChaos)
{
	_maxChaos = maxChaos;
	_targetChaos = chaosLevel;
	_previousChaos = _currentChaos;
	_timeSinceUpdate = 0;
}

void UChaosBarWidget::SetUp()
{
	_OrdinaryColor = _chaosBar->GetFillColorAndOpacity();
}

void UChaosBarWidget::TickBar(float deltaTime)
{
	_timeSinceUpdate += deltaTime;
	
	_currentChaos = FMath::Lerp(_currentChaos, _targetChaos, easeInOutQuint(_timeSinceUpdate));
	
	_chaosBar->SetPercent(_currentChaos/_maxChaos);

	if (_currentChaos >= _maxChaos && !_chaosBarFull)
	{
		ChaosBarFull();
	}

	if (_currentChaos < _maxChaos && _chaosBarFull)
	{
		ChaosBarReset();
	}
}

float UChaosBarWidget::easeInOutQuint(float x)
{
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - FMath::Pow(-2 * x + 2, 5) / 2;
}

void UChaosBarWidget::ChaosBarFull()
{
	_chaosBar->SetFillColorAndOpacity(_chaosBarFullColor);
	_chaosBarFull = true;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Chaos bar full"));
}

void UChaosBarWidget::ChaosBarReset()
{
	_chaosBar->SetFillColorAndOpacity(_OrdinaryColor);
	_chaosBarFull = false;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Chaos bar no longer full"));
}
