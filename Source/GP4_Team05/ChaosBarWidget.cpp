// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaosBarWidget.h"

#include "FrameTypes.h"
#include "Components/ProgressBar.h"

void UChaosBarWidget::UpdateBar(float chaosLevel, float maxChaos)
{
	_maxChaos        = maxChaos;
	_targetChaos     = chaosLevel;
	_previousChaos   = _currentChaos;
	_timeSinceUpdate = 0;
}

void UChaosBarWidget::SetUp()
{
	_OrdinaryColor = _chaosBar->GetFillColorAndOpacity();
}

void UChaosBarWidget::DisableChaosBar(bool isActive)
{
	if (!isActive)
		_chaosBar->SetVisibility(ESlateVisibility::Hidden);
	else
		_chaosBar->SetVisibility(ESlateVisibility::Visible);
}

void UChaosBarWidget::TickBar(float deltaTime)
{
	_timeSinceUpdate += deltaTime;
	
	_currentChaos = FMath::Lerp(_currentChaos, _targetChaos, easeInOutQuint(_timeSinceUpdate));
	_chaosBar->SetPercent(_currentChaos/_maxChaos);
}

float UChaosBarWidget::easeInOutQuint(float x)
{
	return x < 0.5 ? 16 * x * x * x * x * x : 1 - FMath::Pow(-2 * x + 2, 5) / 2;
}

void UChaosBarWidget::ChaosBarFull()
{
	if (!_chaosBarFull) 
	{
		_chaosBar->SetFillColorAndOpacity(_chaosBarFullColor);
		_chaosBarFull = true;
	}
}

void UChaosBarWidget::ChaosBarReset()
{
	_chaosBar->SetFillColorAndOpacity(_OrdinaryColor);
	_currentChaos = 0.0f;
	_targetChaos = 0.0f;
	_timeSinceUpdate = 0.0f;
	_chaosBarFull = false;
}
