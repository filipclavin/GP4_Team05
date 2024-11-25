// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraDataInclude.h"
#include "CharacterStats.generated.h"

class AAuraCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP4_TEAM05_API UCharacterStats : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStats();

	void SetCharacterLevel(int level);

	// The scaling used is percentage > 1 is percent increase < 1 is descrease
	UFUNCTION(BlueprintCallable) void ScaleElementalDamageDealt(ElementTypes elementType, float scaling);
	// The scaling used is percentage > 1 is percent increase < 1 is descrease
	UFUNCTION(BlueprintCallable) void ScaleElementalDamageTaken(ElementTypes elementType, float scaling);
	
	UFUNCTION(BlueprintCallable) void ScaleAllDamageTaken(float scaling);
	UFUNCTION(BlueprintCallable) void ScaleAllDamageDealt(float scaling);
	UFUNCTION(BlueprintCallable) void IncreaseMaxHealth(int healthAmount, bool healCurrentHealth);
	UFUNCTION(BlueprintCallable) void ScaleAttackSpeed(float scaling);
	UFUNCTION(BlueprintCallable) void ScaleMovmementSpeed(float scaling);
	UFUNCTION(BlueprintCallable) void ScaleJumpHeight(float scaling);
	UFUNCTION(BlueprintCallable) void IncreaseCriticalStrikeChance(int amount);

	UFUNCTION(BlueprintCallable) float CalculateDamage(int damage, ElementTypes element, bool& isCrit);

	UFUNCTION(BlueprintCallable) void QueueHeal(int amount);
	UFUNCTION(BlueprintCallable) void QueueDamage(int amount, ElementTypes element, bool crit, AAuraCharacter* attacker, bool selfDamageTaken = false);


	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool  _isInvincible     = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int   _maxHealth     = 100;
	UPROPERTY(BlueprintReadOnly)			    int   _currentHealth = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _movementSpeed = 600;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _jumpSpeed     = 420;
	
	// Damage calculation 
	// (Damage * ElementDamageDealt) * AllDamageDealt  * crit = true ? 2 : 1 -> (DamageTaken * ElementDamageTaken) * AllDamageTaken

	// Scalings
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int   _critStrikeChance = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _healingTaken	    = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _allDamageTaken   = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _allDamageDealt   = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _attackSpeed      = 1.0f;
	// How long it takes for you abilities to recharge.
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _cooldownRecovery = 1.0f;
	/*
		In Order of:
		PHYSICAL,
		FIRE,
		LIGHTNING
		BLOOD
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<float>  _elementDamageTaken	= {};
	/*
		In Order of:
		PHYSICAL,
		FIRE,
		LIGHTNING,
		BLOOD
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<float>  _elementDamageDealt	= {};

	
	UPROPERTY(BlueprintReadOnly)  bool _isAlive = true;
	
	UPROPERTY(EditAnywhere) float _healthScaling          = 1.0f;
	UPROPERTY(EditAnywhere) float _speedScaling           = 1.0f;
	UPROPERTY(EditAnywhere) float _damageDealtScaling     = 1.0f;
	// For damage reduction it is from 1.0 -> 0.0f
	UPROPERTY(EditAnywhere) float _damageReductionScaling = 1.0f;

protected:
	AAuraCharacter* _parent;

	struct IntakeData 
	{
		enum class Type
		{
			Damage,
			Heal,
		};

		AAuraCharacter*  _attacker = nullptr;
		bool			 _selfDamageTaken = false; 
		Type		     _type;
		ElementTypes     _element;
		bool			 _isCrit = false;
		int			     _amount;
	};
			
	TArray<IntakeData> _intakeQueue;

	bool IsCriticalStrike();

	int _currentLevel;

	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
