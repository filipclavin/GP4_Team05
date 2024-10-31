// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDataInclude.h"
#include "CharacterStats.h"
#include "GameFramework/Character.h"
#include "AuraCharacter.generated.h"

class UAura;
class AAuraHandler;
class UCharacterMovementComponent;

UCLASS()
class GP4_TEAM05_API AAuraCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnDamageIntake(int amount, ElementTypes elementType, bool crit);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void OnHealIntake(int amount);
	void OnDamageIntake_Implementation(int amount, ElementTypes elementType, bool crit){}
	void OnHealIntake_Implementation(int amount){}

	UFUNCTION(BlueprintCallable) bool HasAuraWithID(int id);
	UFUNCTION(BlueprintCallable) bool HasAuraWithName(FString name);

	// Queus damage into a list that gets handled at the end of the frame
	UFUNCTION(BlueprintCallable) void QueueHeal(int amount);
	UFUNCTION(BlueprintCallable) int QueueDamage(int amount, ElementTypes element, AAuraCharacter* attacker = nullptr, bool selfDamageTaken = false);

	UFUNCTION(BlueprintCallable) UCharacterStats* GetStats()     { return _combinedStats; }
	UFUNCTION(BlueprintCallable) UCharacterStats* GetBaseStats() { return _baseStats; }

	UFUNCTION(BlueprintCallable) const TArray<UAura*>& GetAffectedBuffs()   { return _auraList[BUFF]; }
	UFUNCTION(BlueprintCallable) const TArray<UAura*>& GetAffectedDebuffs() { return _auraList[DEBUFF]; }


	void UpdateAurasOnAttackHits(AAuraCharacter* target, AuraAttackType attackType, int damageDone);
	void UpdateAurasOnAttackCast(AuraAttackType attackType);
	void UpdateAurasOnDamageTaken(AAuraCharacter* attacker);
	bool IsPlayer() { return _isPlayer; }

protected:
	virtual void BeginPlay() override;

	void AddAura(UAura* aura);
	// Saves the aura ID and where it is in the buff/debuff list for quick and easy access. 

	TMap<INT32  /*Aura ID*/,   TPair<AuraType, INT32>> _auraIdMap;
	TMap<FString/*Aura Name*/, TPair<AuraType, INT32>> _auraNameMap;

	TStaticArray<TArray<UAura*>, AuraType::AURA_TYPE_COUNT> _auraList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) AAuraHandler*  _auraHandler = nullptr;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere) UCharacterStats*			 _baseStats;
	UPROPERTY(BlueprintReadWrite)				UCharacterStats*			 _combinedStats;
											    UCharacterMovementComponent* _movementComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)  bool _isPlayer = false;

	// Ability list, Auras can fetch this data and change the settings of these abilities

	// Aura updates in the order of Buffs -> Debuffs 
	UFUNCTION(BlueprintCallable) void UpdateAuras(const float deltaTime);
	UFUNCTION(BlueprintCallable) virtual void Die();

	void ResetToBaseStats();
private:
	friend class AAuraHandler;

	UAura* AffectedByAura(const int id); // Used by AuraHandler
	void RemoveAura(const size_t index, const AuraType type);


public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Kill() { Die(); }

};
