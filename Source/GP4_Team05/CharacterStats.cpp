#include "CharacterStats.h"
#include "AuraCharacter.h"

UCharacterStats::UCharacterStats()
{
	PrimaryComponentTick.bCanEverTick = true;
	Super::SetTickGroup(ETickingGroup::TG_PostPhysics);

	for (uint8 i = 0; i < ElementTypes::ELEMENT_TYPE_COUNT; i++) 
	{
		_elementDamageDealt.Add(1.0f);
		_elementDamageTaken.Add(1.0f);	
	}			
}

void UCharacterStats::SetCharacterLevel(int level)
{
	int levelDelta = level - _currentLevel;
	for (int i = 0; i < levelDelta; i++)
	{
		_maxHealth       = FMath::RoundToInt(_maxHealth * _healthScaling);
		_allDamageTaken *= _damageReductionScaling;
		_movementSpeed  *= _speedScaling;
		_allDamageDealt *= _damageDealtScaling;
	}
}

void UCharacterStats::ScaleElementalDamageDealt(ElementTypes elementType, float scaling)
{
	if (elementType != ELEMENT_TYPE_COUNT && elementType!=MAGIC)
		_elementDamageDealt[elementType] *= scaling;
}

void UCharacterStats::ScaleElementalDamageTaken(ElementTypes elementType, float scaling)
{
	if (elementType != ELEMENT_TYPE_COUNT && elementType!= MAGIC)
		_elementDamageTaken[elementType] *= scaling;
}

void UCharacterStats::ScaleAllDamageTaken(float scaling)
{
}

void UCharacterStats::ScaleAllDamageDealt(float scaling)
{
}

void UCharacterStats::IncreaseMaxHealth(int healthAmount, bool healCurrentHealth)
{
}

void UCharacterStats::ScaleAttackSpeed(float scaling)
{
}

void UCharacterStats::ScaleMovmementSpeed(float scaling)
{
	_movementSpeed *= scaling;
}

void UCharacterStats::IncreaseCriticalStrikeChance(int amount)
{
}

void UCharacterStats::ScaleJumpHeight(float scaling)
{
}

float UCharacterStats::CalculateDamage(int damage, ElementTypes element, bool& isCrit)
{
	float newDamage = damage * (_elementDamageDealt.IsEmpty() ? 1.0f : _elementDamageDealt[element]);
	newDamage *= _allDamageDealt;
	isCrit = IsCriticalStrike();
	newDamage *= isCrit ? 2 : 1;
	return newDamage;
}

void UCharacterStats::QueueHeal(int amount)
{
	IntakeData data;
	data._amount = amount;
	data._type = IntakeData::Type::Heal;

	_intakeQueue.Add(data);
}

void UCharacterStats::QueueDamage(int amount, ElementTypes element, bool crit, AAuraCharacter* attacker, bool selfDamageTaken)
{
	IntakeData data;
	data._amount   = amount;
	data._type     = IntakeData::Type::Damage;
	data._element  = element;
	data._isCrit   = crit;
	data._attacker = attacker;
	data._selfDamageTaken = selfDamageTaken;
	_intakeQueue.Add(data);
}

bool UCharacterStats::IsCriticalStrike()
{
	if (_critStrikeChance > 0) 
	{
		if (_critStrikeChance >= 100)
			return true;

		int crit = FMath::RandRange(0, 100);
		if (crit <= _critStrikeChance)
			return true;
	}
	return false;
}

void UCharacterStats::BeginPlay()
{
	Super::BeginPlay();
	_parent = Cast<AAuraCharacter>(GetOwner());
}

void UCharacterStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_currentHealth > _maxHealth)
		_currentHealth = _maxHealth;

	if (_isAlive) 
	{
		for (INT32 i = 0; i < _intakeQueue.Num(); i++)
		{
			IntakeData& intake = _intakeQueue[i];
		
			float newAmount   = 0.0f;
			int roundedAmount = 0;
			switch (intake._type)
			{
			case IntakeData::Type::Damage:
				newAmount = (intake._amount * _elementDamageTaken[intake._element]) * _allDamageTaken;
				roundedAmount = FMath::RoundToInt(newAmount);
				_currentHealth -= roundedAmount;
				if(!intake._selfDamageTaken)
				{
					_parent->OnDamageIntake(roundedAmount, intake._element, intake._isCrit);
					if(intake._attacker)
						_parent->UpdateAurasOnDamageTaken(intake._attacker);		
				}
				break;
			case IntakeData::Type::Heal:
				newAmount = (intake._amount * _healingTaken);
				roundedAmount = FMath::RoundToInt(newAmount);
				_currentHealth += roundedAmount;
				_currentHealth = _currentHealth > _maxHealth ? _maxHealth : _currentHealth;
				_parent->OnHealIntake(roundedAmount);
				break;
			}

			if (_currentHealth <= 0)
			{
				if (_isInvincible)
					_currentHealth = 1;
				else 
				{
					Cast<AAuraCharacter>(GetOwner())->Kill();
					break;
				}
			}
		}
		
		if(!_intakeQueue.IsEmpty())
		_intakeQueue.Empty();
	}
	else 
	{
		if(!_intakeQueue.IsEmpty())
		_intakeQueue.Empty();
	}
}

