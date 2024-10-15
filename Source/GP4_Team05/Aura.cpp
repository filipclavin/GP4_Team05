#include "Aura.h"

UAura::UAura()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UAura::InitializeBasicProperties()
{
	_currentDuration = _duration;
	
	if (_duration != FLT_MAX && _tickCounter >= 1) {
		_tickDuration = _duration / _tickCounter;
		_currentTick = _tickDuration;
	}
}
void UAura::DecreaseDuration(const float deltaTime)
{
	if (_duration != FLT_MAX)
	{
		_currentDuration -= deltaTime;
	}

	if (_tickDuration != FLT_MAX)
	{
		_currentTick = _currentTick <= 0.0f ? _tickDuration : _currentTick;
		_currentTick -= deltaTime;
	}
}

void UAura::CreatePooledAuras_Implementation(AAuraHandler* handler, AActor* pooledActor)
{
	if (GEngine) {
		FString s = _auraName + " CreatePoolAuras function is not overriden and wont spawn any auras!";
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, s);
	}
}

void UAura::UpdateAuraPool(UAura* newAura)
{
	newAura->InitializeBasicProperties();
	_auraPool.Add(newAura);
}

void UAura::ResetAura(const UAura* aura)
{
	if (_id == -1) 
	{
		_id              = aura->_id;
		_icon			 = aura->_icon;
		_auraName		 = aura->_auraName;
		_type			 = aura->_type;
		_tickCounter     = aura->_tickCounter;
	}
	
	_duration		 = aura->_duration;
	_tickDuration    = aura->_tickDuration;
	_currentTick     = aura->_currentTick;
	_currentDuration = aura->_currentDuration;
	_stackCount      = aura->_stackCount;

	_affected = nullptr;
	_caster   = nullptr;
}


