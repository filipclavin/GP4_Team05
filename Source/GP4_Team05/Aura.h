#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraDataInclude.h"
#include "Aura.generated.h"

class AAuraCharacter;
class AAuraHandler;
class UTexture2D;

UCLASS(Blueprintable, ClassGroup = (Auras), meta = (BlueprintSpawnableComponent))
class GP4_TEAM05_API UAura : public UActorComponent
{
	GENERATED_BODY()
public:
	UAura();

	// Called when aura is casted.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraCast(AAuraCharacter* caster); 
	// Called when the aura is added in buff/debuff list. 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraAdd(AAuraCharacter*  affectedChar);
	// Called every frame when active on an AuraCharacter.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraUpdate(const float deltaTime);
	// Called on every AuraTick = Duration / Tick Counter. Current Tick counts down automatically 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraTick();
	// Called when the aura is removed from buff/debuff list or when AuraCharacter is deactivated/dead.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraRemoved();
	// Called if an affected AuraCharacter already has this aura (based on Aura ID).
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraExists();
	// Called when AuraCharacter hits with an ability or regular attack. Note: it is not possible to specify what attack at the moment
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAttackHit(AAuraCharacter* targetHit);

	void OnAttackHit_Implementation(AAuraCharacter* targetHit)  { targetHit;    }
	void OnAuraCast_Implementation(AAuraCharacter* caster)      { caster;	    }
	void OnAuraAdd_Implementation(AAuraCharacter* affectedChar) { affectedChar; }
	void OnAuraUpdate_Implementation(const float deltaTime)     { deltaTime;    }
	void OnAuraRemoved_Implementation()						    {			    }
	void OnAuraTick_Implementation()						    {			    }
	void OnAuraExists_Implementation()							{			    }

	// Don't Update _currentDuration if Aura is infinite!
	void DecreaseDuration(const float deltaTime);

	UFUNCTION(BlueprintCallable) const AuraType GetType()  { return _type; }
	UFUNCTION(BlueprintCallable) const FString  AuraName() { return _auraName; }
	UFUNCTION(BlueprintCallable) const int      GetID()	   { return _id; }

	const float GetTickDuration() { return _currentTick; }
	const float GetDuration()     { return _currentDuration; }

	UFUNCTION(BlueprintCallable) 
	bool IsAuraActive()   { return _isActive;  }
	void ActivateAura()	  { _isActive = true;  }
	void DeactivateAura() { _isActive = false; }

	UTexture2D* GetIcon() { return _icon; }

protected:
	UPROPERTY(BlueprintReadWrite) AAuraCharacter* _affected = nullptr;
	UPROPERTY(BlueprintReadWrite) AAuraCharacter* _caster   = nullptr;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void CreatePooledAuras(AActor* pooledActor);
	void CreatePooledAuras_Implementation(AActor* pooledActor);
	UFUNCTION(BlueprintCallable) void UpdateAuraPool(UAura* newAura);

	int  _id   = -1;
	bool _isActive  = false;
	// General Aura values, These values do not need to be used by all auras

	// Name of aura.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)  FString _auraName;
	// Icon of the aura, used for UI.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)  UTexture2D* _icon;
	// Auras updates in the order of Buff -> Debuffs and to differentiate them in UI 
	UPROPERTY(BlueprintReadOnly, EditAnywhere)  TEnumAsByte<AuraType> _type = AuraType::BUFF;
	// Duration of aura, FLT_MAX = Infinite duration.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float   _duration    = FLT_MAX; 	
	// Tick Counter for how many times the aura should tick during it's duration.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int     _tickCounter = 0;    
	
	// Private aura values, used to affect the target entity after a certain duration.

	// The tick duration of the aura, decided by Duration / TickCounter (if aura is infinite then define TickDuration manually) 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)    float    _tickDuration  = FLT_MAX;
	// Current duration of the aura
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) float    _currentDuration = 0.0f;
	// Current tick of the aura, used to make the aura do something when a tick has reached 0
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) float    _currentTick     = 0.0f;
	 // How many stacks the aura has
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) int      _stackCount	= 1;

	// Pooling information
	// How much of this aura AuraHandler will pool, auras that appear less or are much shorter should have a lower pool count.
	UPROPERTY(EditAnywhere) int _poolCount = 10;
	TArray<UAura*> _auraPool;
	void ResetAura(const UAura* aura);

private:
	friend class AAuraHandler;
	
	// Default initilization for every aura, called in AuraHandler::FetchAllAurasAttached.
	void InitializeBasicProperties();
};
