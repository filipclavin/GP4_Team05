
#include "AuraCharacter.h"
#include "Aura.h"
#include "AuraHandler.h"
#include "Gameframework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

AAuraCharacter::AAuraCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_baseStats		   = CreateDefaultSubobject<UCharacterStats>("Base Stats");
	_combinedStats	   = CreateDefaultSubobject<UCharacterStats>("Combined Stats");
}

bool AAuraCharacter::HasAuraWithID(int id)
{
	if (_auraIdMap.Contains(id))
		return true;
	return false;
}

bool AAuraCharacter::HasAuraWithName(FString name)
{
	if (_auraNameMap.Contains(name))
		return true;
	return false;
}

void AAuraCharacter::AddAura(UAura* aura)
{
	if (!aura) {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Aura applied is null!"));
		return;
	}

	INT32 i = _auraList[aura->GetType()].Add(aura);
	_auraIdMap.Add(aura->GetID(), { aura->GetType(), i });
	_auraNameMap.Add(aura->AuraName(), { aura->GetType(), i });

	aura->OnAuraAdd(this);
}


void AAuraCharacter::QueueHeal(int amount)
{
	_combinedStats->QueueHeal(amount);
}

void AAuraCharacter::QueueDamage(int amount, ElementTypes element, UCharacterStats* stats)
{
	_combinedStats->QueueDamage(amount, element, stats);
}

void AAuraCharacter::UpdateAurasOnAttackHits(AAuraCharacter* target, AuraAttackType attackType)
{
	TArray<UAura*>& auras = _auraList[EFFECT];
	for (INT32 i = 0; i < _auraList[EFFECT].Num(); i++)
	{
		if(attackType == auras[i]->GetAuraAttackType())
			auras[i]->OnAuraAttackHit(target);		
	}
}

void AAuraCharacter::UpdateAurasOnAttackCast(AuraAttackType attackType)
{
	TArray<UAura*>& auras = _auraList[EFFECT];
	for (INT32 i = 0; i < _auraList[EFFECT].Num(); i++)
	{
		if (attackType == auras[i]->GetAuraAttackType())
			auras[i]->OnAuraCast(this);
	}
}

// Called when the game starts or when spawned
void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

void AAuraCharacter::UpdateAuras(const float deltaTime)
{
	if (_characterIsActive) {
		if (!_combinedStats->_isAlive) {
			OnDeath();
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "DEAD!");
			_characterIsActive = false;
		}
		else
		{
			ResetToBaseStats();
			
			for (uint8 i = 0; i < AURA_TYPE_COUNT; i++)
			{
				TArray<UAura*>& list = _auraList[i];
				for (INT32 j = 0; j < list.Num(); j++)
				{
					list[j]->DecreaseDuration(deltaTime);
					if(i != AuraType::EFFECT)
					{
						list[j]->OnAuraUpdate(deltaTime);
						if (list[j]->GetTickDuration() <= 0.0f)
							list[j]->OnAuraTick();
					}
				
					if (list[j]->GetDuration() <= 0.0f || list[j]->IsAuraActive() == false)
						RemoveAura(j, AuraType(i));
				}
			}

			// Fetch component once in UpdateAuras
			if (!_movementComponent)
				_movementComponent = GetComponentByClass<UCharacterMovementComponent>();

			// Apply our movement stats to CharacterMovementComponent
			if(_movementComponent){
				_movementComponent->JumpZVelocity = _combinedStats->_jumpSpeed;
				_movementComponent->MaxWalkSpeed  = _combinedStats->_movementSpeed;
			}
		}
	}
}

void AAuraCharacter::OnDeath()
{
	for (uint8 i = 0; i < AURA_TYPE_COUNT; i++)
	{
		TArray<UAura*>& list = _auraList[i];
		for (INT32 j = list.Num() - 1; j > -1; j--) 
		{
			RemoveAura(j, AuraType(i));
		}
	}
}

UAura* AAuraCharacter::AffectedByAura(const int id)
{
	if (_auraIdMap.Contains(id)) 
	{
		const AuraType type = _auraIdMap[id].Key;
		return _auraList[type][_auraIdMap[id].Value];
	}
		
	return nullptr;
}


void AAuraCharacter::RemoveAura(const size_t index, const AuraType type)
{
	TArray<UAura*>& typeList = _auraList[type];

	if(type != EFFECT)
		typeList[index]->OnAuraRemoved();

	typeList[index]->DeactivateAura();

	INT32 id     = typeList[index]->GetID();
	FString name = typeList[index]->AuraName();
	_auraIdMap.Remove(id);
	_auraNameMap.Remove(name);

	if (typeList.Num() > 1 && index < (typeList.Num() - 1)) {
		typeList[index] = typeList[typeList.Num() - 1];
		typeList.Pop();
		id   = typeList[index]->GetID();
		name = typeList[index]->AuraName();
		_auraIdMap[id].Key   = typeList[index]->GetType();
		_auraIdMap[id].Value = index;

		_auraNameMap[name].Key   = typeList[index]->GetType();
		_auraNameMap[name].Value = index;
	}
	else
		typeList.Pop();
}

void AAuraCharacter::ResetToBaseStats()
{
	_combinedStats->_maxHealth			= _baseStats->_maxHealth;
	_combinedStats->_allDamageDealt		= _baseStats->_allDamageDealt;
	_combinedStats->_allDamageTaken		= _baseStats->_allDamageTaken;
	_combinedStats->_movementSpeed      = _baseStats->_movementSpeed;
	_combinedStats->_elementDamageDealt = _baseStats->_elementDamageDealt;
	_combinedStats->_elementDamageTaken = _baseStats->_elementDamageTaken;
	_combinedStats->_jumpSpeed			= _baseStats->_jumpSpeed;
}

void AAuraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

