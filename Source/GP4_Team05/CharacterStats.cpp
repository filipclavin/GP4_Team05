#include "CharacterStats.h"

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

// Need to find a way to calculate this information after all buffs have been applied, as i said would be much
// easier if Unreal acutally hade multiple different updates during the render frame that we could use
int UCharacterStats::CalculateDamage(int damage, ElementTypes element)
{
	float newDamage = ((damage * _elementDamageDealt[element]) * _allDamageDealt) * IsCriticalStrike() ? 2 : 1;
	return RoundToInt(newDamage);
}

void UCharacterStats::QueueHeal(int amount)
{
	IntakeData data;
	data._amount = amount;
	data._type = IntakeData::Type::Heal;

	_intakeQueue.Add(data);
}

void UCharacterStats::QueueDamage(int amount, ElementTypes element)
{
	IntakeData data;
	data._amount = amount;
	data._type = IntakeData::Type::Damage;
	data._element = element;
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
}

void UCharacterStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_isAlive) 
	{
		for (INT32 i = 0; i < _intakeQueue.Num(); i++)
		{
			IntakeData& intake = _intakeQueue[i];
		
			float newAmount = 0.0f;
			switch (intake._type)
			{
			case IntakeData::Type::Damage:
				newAmount = (intake._amount * _elementDamageTaken[intake._element]) * _allDamageTaken;
				_currentHealth -= RoundToInt(newAmount);
				break;
			case IntakeData::Type::Heal:
				newAmount = (intake._amount * _healingTaken);
				_currentHealth += RoundToInt(newAmount);
				break;
			}

			if (_currentHealth <= 0)
			{
				if (_isInvincible)
					_currentHealth = 1;
				else 
				{
					_isAlive = false;
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Aura Character is dead!"));
				}
				break;
			}

			if (_currentHealth > _maxHealth)
				_currentHealth = _maxHealth;
		}
		_intakeQueue.Empty();
	}
}

