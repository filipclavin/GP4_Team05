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
		_maxHealth      *= _healthScaling;
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

int UCharacterStats::CalculateDamage(int damage, ElementTypes element)
{
	float newDamage = ((damage * _elementDamageDealt.IsEmpty() ? 1.0f : _elementDamageDealt[element]) * _allDamageDealt) * IsCriticalStrike() ? 2 : 1;
	return RoundToInt(newDamage);
}

void UCharacterStats::QueueHeal(int amount)
{
	IntakeData data;
	data._amount = amount;
	data._type = IntakeData::Type::Heal;

	_intakeQueue.Add(data);
}

void UCharacterStats::QueueDamage(int amount, ElementTypes element, UCharacterStats* stats)
{
	IntakeData data;
	data._amount = amount;
	data._type = IntakeData::Type::Damage;
	data._element = element;
	data._stats = stats;
	_intakeQueue.Add(data);
}

int UCharacterStats::RoundToInt(float amount)
{
	amount = amount + 0.5f - (amount < 0);
	return int(amount);
}

bool UCharacterStats::IsCriticalStrike()
{
	if (_critStrikeChance > 0.0f) 
	{
		if (_critStrikeChance >= 100.0f)
			return true;

		float crit = FMath::RandRange(0.0f, 100.0f);
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
		
			float newAmount = 0.0f;
			switch (intake._type)
			{
			case IntakeData::Type::Damage:
				if(intake._stats)
					newAmount = (intake._stats->CalculateDamage(intake._amount, intake._element) * _elementDamageTaken[intake._element]) * _allDamageTaken;
				else
					newAmount = (intake._amount * _elementDamageTaken[intake._element]) * _allDamageTaken;
				
				intake._stats = nullptr;
				_currentHealth -= RoundToInt(newAmount);
				_parent->OnDamageIntake();
				break;
			case IntakeData::Type::Heal:
				newAmount = (intake._amount * _healingTaken);
				_currentHealth += RoundToInt(newAmount);
				_parent->OnHealIntake();
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
}

