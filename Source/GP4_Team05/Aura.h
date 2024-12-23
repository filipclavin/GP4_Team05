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
	// Called when the aura is added to a AuraCharacter. 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraAdd(AAuraCharacter*  affectedChar);
	// Called every frame when active on an AuraCharacter.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraUpdate(const float deltaTime);
	// Called on every AuraTick = Duration / Tick Counter. Current Tick counts down automatically 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraTick();
	// Called when the aura is removed from buff/debuff list or when AuraCharacter is deactivated/dead.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraRemoved();
	// Called if an affected AuraCharacter already has this aura (based on Aura ID).
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraExists();
	// Called when AuraCharacter hits with an ability or regular attack. Attack is specified by AuraAttackType
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraAttackHit(AAuraCharacter* targetHit, int damageDone);
	// Called when AuraCharacter has gotten hit by an attack. Does not include self inflicted or enviromental damage.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnAuraDamageTaken(AAuraCharacter* attacker);

	void OnAuraDamageTaken_Implementation(AAuraCharacter* attacker)				   { attacker;				 }
	void OnAuraAttackHit_Implementation(AAuraCharacter* targetHit, int damageDone) { targetHit;  damageDone; }
	void OnAuraCast_Implementation(AAuraCharacter* caster)		                   { caster;	             }
	void OnAuraAdd_Implementation(AAuraCharacter* affectedChar)	                   { affectedChar;           }
	void OnAuraUpdate_Implementation(const float deltaTime)		                   { deltaTime;              }
	void OnAuraRemoved_Implementation()							                   {			             }
	void OnAuraTick_Implementation()							                   {			             }
	void OnAuraExists_Implementation()							                   {			             }

	// Don't Update _currentDuration if Aura is infinite!
	void DecreaseDuration(const float deltaTime);

	UFUNCTION(BlueprintCallable) const AuraType     GetType()        { return _type; }
	UFUNCTION(BlueprintCallable) const ElementTypes GetElementType() { return _auraElementType; }
	UFUNCTION(BlueprintCallable) const FString      AuraName()	     { return _auraName; }
	UFUNCTION(BlueprintCallable) const int          GetID()		     { return _id; }

	const AuraAttackType GetAuraAttackType()			   { return _onAttackType; };

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

	int  _id		= -1;
	bool _isActive  = false;
	// General Aura values, These values do not need to be used by all auras

	// Name of aura.
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FString					   _auraName = "";
	// Description of Aura
	UPROPERTY(BlueprintReadOnly, EditAnywhere) FString					   _auraDescription = "";
	// Icon of the aura, used for UI, Auras with no icon will not appear in the UI.
	UPROPERTY(BlueprintReadOnly, EditAnywhere) UTexture2D*				   _icon = nullptr;
	// Auras updates in the order of Buff -> Debuffs and to differentiate them in UI 
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TEnumAsByte<AuraType>	   _type = AuraType::BUFF;
	// This is only for ON_HIT auras, BUFF and DEBUFF are not affected by this. 
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TEnumAsByte<AuraAttackType> _onAttackType = AuraAttackType::MELEE;
	// Duration of aura, FLT_MAX = Infinite duration.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float					   _duration = FLT_MAX; 	
	// Tick Counter for how many times the aura should tick during it's duration.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int						   _tickCounter = 0;    
	UPROPERTY(BlueprintReadOnly)				AAuraHandler*			   _auraHandler;
	// The tick duration of the aura, decided by Duration / TickCounter (if aura is infinite then define TickDuration manually) 
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float				       _tickDuration = FLT_MAX;
	// Current duration of the aura
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) float				   _currentDuration = 0.0f;
	// Current tick of the aura, when it reachers 0 OnAuraTick is called
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere) float				   _currentTick = 0.0f;
	 // How many stacks the aura has
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int					       _maxStackCount = 1;
	UPROPERTY(BlueprintReadWrite)				int					       _currentStack = 0;

	
	// If true this aura has a chance to spawn as an Interactable pickup at room completion. 
	UPROPERTY(BlueprintReadOnly, EditAnywhere)  bool		_auraSpawnAsInteractable = false;
	// Check to see if aura can stack, mostly used for InteractableSelector to decide if we can spawn duplicates.
	UPROPERTY(BlueprintReadOnly, EditAnywhere) bool           _canStack = true;
	// This is for when spawning as AuraInteractable to set it's color and effect for players to understand what type of element it affects. 
	UPROPERTY(BlueprintReadOnly, EditAnywhere) TEnumAsByte<ElementTypes>   _auraElementType = ElementTypes::MAGIC;
		
	// Pooling information
	// How much of this aura AuraHandler will pool, auras that appear less or are much shorter should have a lower pool count.
	UPROPERTY(EditAnywhere) int _poolCount = 10;
	TArray<UAura*>				_auraPool;
	void ResetAura(const UAura* aura);

private:
	friend class AAuraHandler;
	
	// Default initilization for every aura, called in AuraHandler::FetchAllAurasAttached.
	void InitializeBasicProperties();
};
