#include "AuraHandler.h"
#include "AuraCharacter.h"
#include "Aura.h"

AAuraHandler::AAuraHandler()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Returns aura ID based on name, if it doesn't find specifed aura it returns INT32_MAX
int AAuraHandler::GetAuraIDbyName(FString name)
{
	if (_auraNameMap.Contains(name))
		return _auraList[_auraNameMap[name]]->_id;
	return INT32_MAX;
}

// Returns aura Name bases on ID, if it doesn't find specifed aura it returns "Invalid Aura"
FString AAuraHandler::GetAuraNameByID(int id)
{
	if (id >= 0 && id < _auraList.Num())
		return _auraList[id]->_auraName;
	return "Invalid Aura";
}

// Cast aura based on ID
void AAuraHandler::CastAuraByID(const int id, AAuraCharacter* target, AAuraCharacter* caster)
{
	if (id >= 0 && id < _auraList.Num()) 
	{
		UAura* affectedAura = target->AffectedByAura(id);
		if(affectedAura)
		{
			affectedAura->OnAuraCast(caster);
			affectedAura->OnAuraExists();
		}
		else 
		{
			UAura* aura = GetPooledAura(_auraList[id]);
			aura->Activate();
			aura->OnAuraCast(caster);
			target->AddAura(aura);
		}
	}
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Invalid Aura ID or AuraList is empty!"));
}

//Cast aura based on Name
void AAuraHandler::CastAuraByName(FString name, AAuraCharacter* target, AAuraCharacter* caster)
{
	if(_auraNameMap.Contains(name))
	{
		INT32 id = _auraNameMap[name];
		UAura* affectedAura = target->AffectedByAura(id);
		if (affectedAura)
		{
			// Effects call OnAuraCast on abilities instead.
			if(affectedAura->GetType() != EFFECT)
				affectedAura->OnAuraCast(caster);
			affectedAura->OnAuraExists();
		}
		else
		{
			UAura* aura = GetPooledAura(_auraList[id]);
			if (aura) 
			{
				aura->ActivateAura();
				if(aura->GetType() != EFFECT)
					aura->OnAuraCast(caster);
				target->AddAura(aura);
			}
			else
				if(GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No auras available or Aura does not exist!"));
		}
	}
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Invalid Aura Name or AuraList is empty!"));
}

FString AAuraHandler::GetAuraDescription(FString nameOfAura)
{
	if (_auraNameMap.Contains(nameOfAura)) {
		INT32 id = _auraNameMap[nameOfAura];
		return _auraList[id]->_auraDescription;
	}
	return "Aura not found :(";
}

void AAuraHandler::RemovePickupableAura(FString auraName)
{
	// If aura cannot stack, remove it from the spawn pool if the player has picked it up.
	if (!_auraList[_auraNameMap[auraName]]->_canStack)
	{
		INT32 index = -1;
		if (_auraSpawnList.Find(auraName, index)) 
		{
			_auraSpawnList[index] = _auraSpawnList[_auraSpawnList.Num() - 1];
			_auraSpawnList.Pop();
		}
	}
}

TArray<UAura*> AAuraHandler::GetRandomAuraFromSpawnList()
{
	if(!_auraSpawnList.IsEmpty())
	{
		TArray<UAura*> auraRandomList = TArray<UAura*>();
		for (INT32 i = 0; i < 3; i++)
		{
			INT32 randomIndex = 0;
			UAura* aura = GetRandomAura(randomIndex);

			for (INT32 j = 0; j < auraRandomList.Num(); j++)
			{
				if (j != i) 
				{
					if (aura == auraRandomList[j]) {
						aura = GetRandomAura(randomIndex);
					}
				}
			}

			auraRandomList.Add(aura);
		}
	
		return auraRandomList;
	}
	return TArray<UAura*>();
}

void AAuraHandler::FetchAllAurasAttached()
{
	GetComponents<UAura>(_auraList);

	// Assign Aura names and ID's:
	for (size_t i = 0; i < _auraList.Num(); i++)
	{
		UAura* aura = _auraList[i];
		aura->_id = i;
		aura->InitializeBasicProperties();
		_auraNameMap.Add(aura->_auraName, i);

		// Add Aura to spawn interactable list if it is set 
		if (aura->_auraSpawnAsInteractable)
			_auraSpawnList.Add(aura->AuraName());

		// check what poolCount is higher, id aura->_poolCount is less than Minimun than it will override it!
		int poolCount = aura->_poolCount < _minimunPoolCount ? _minimunPoolCount : aura->_poolCount;
		aura->_auraPool.Reserve(poolCount); // reserve pool list
		for (size_t j = 0; j < poolCount; j++)
		{
			// Create the pooling for the specific aura subclass
			aura->CreatePooledAuras(_auraPoolingActor);
			aura->_auraHandler = this;
		}
	}
}

UAura* AAuraHandler::GetRandomAura(INT32& randomIndex)
{
	randomIndex = FMath::RandRange(0, _auraSpawnList.Num() - 1);
	return _auraList[_auraNameMap[_auraSpawnList[randomIndex]]];
}

UAura* AAuraHandler::GetPooledAura(UAura* aura)
{
	TArray<UAura*>& pool = aura->_auraPool;

	for (size_t i = 0; i < pool.Num(); i++)
	{
		// Reset Aura when we fetch it so it's ready for use.
		if (!pool[i]->_isActive) {
			pool[i]->ResetAura(aura);
			return pool[i];
		}
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "No Available auras for " + aura->_auraName);

	return nullptr;
}

void AAuraHandler::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

