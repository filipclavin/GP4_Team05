// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraDataInclude.h"
#include "CharacterStats.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP4_TEAM05_API UCharacterStats : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterStats();

	void SetCharacterLevel(int level);

	UFUNCTION(BlueprintCallable) int CalculateDamage(int damage, ElementTypes element);

	UFUNCTION(BlueprintCallable) void QueueHeal(int amount);
	UFUNCTION(BlueprintCallable) void QueueDamage(int amount, ElementTypes element, UCharacterStats* stats);


	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool  _isInvincible     = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int   _maxHealth     = 100;
	UPROPERTY(BlueprintReadOnly)			    int   _currentHealth = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _movementSpeed = 600;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _jumpSpeed     = 420;
	
	// Damage calculation 
	// (Damage * ElementDamageDealt) * AllDamageDealt  * crit = true ? 2 : 1 -> (DamageTaken * ElementDamageTaken) * AllDamageTaken

	// Scalings
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _critStrikeChance = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _healingTaken	    = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _allDamageTaken   = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float _allDamageDealt   = 1.0f;
	/*
		In Order of:
		PHYSICAL,
		FIRE,
		LIGHTNING
		WATER,
		EARTH,
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<float>  _elementDamageTaken	= {};
	/*
		In Order of:
		PHYSICAL,
		FIRE,
		LIGHTNING,
		WATER,
		EARTH,
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<float>  _elementDamageDealt	= {};

	UPROPERTY(BlueprintReadOnly)  bool _isAlive = true;
	
	UPROPERTY(EditAnywhere) float _healthScaling      = 1.0f;
	UPROPERTY(EditAnywhere) float _speedScaling       = 1.0f;
	UPROPERTY(EditAnywhere) float _damageDealtScaling = 1.0f;


protected:
	struct IntakeData 
	{
		enum class Type
		{
			Damage,
			Heal,
		};

		Type		     _type;
		ElementTypes     _element;
		UCharacterStats* _stats = nullptr;
		int			     _amount;
	};
			
	TArray<IntakeData> _intakeQueue;

	int RoundToInt(float amount);
	bool IsCriticalStrike();

	int _currentLevel;

	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
