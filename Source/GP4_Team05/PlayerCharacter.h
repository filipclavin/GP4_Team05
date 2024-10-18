// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacter.h"
#include "InputActionValue.h"
#include "ProjectileBaseClass.h"
#include "PlayerCharacter.generated.h"

class AProjectileBaseClass;
class USphereComponent;
class USpringArmComponent;
class UBoxComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

USTRUCT()
struct FPlayerUpgrades
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly) int   LightMeleeDamage = 0;
	UPROPERTY(EditDefaultsOnly) int	  HeavyMeleeDamage = 0;
	UPROPERTY(EditDefaultsOnly) float BloodAbsorb	   = 0;
	UPROPERTY(EditDefaultsOnly) int   DashSpeed		   = 0;
};
/**
 * 
 */
UCLASS()
class GP4_TEAM05_API APlayerCharacter : public AAuraCharacter
{
	GENERATED_BODY()
	APlayerCharacter();

	
private:

	UPROPERTY(EditAnywhere, Category=input) UInputAction* _move;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _jump;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _look;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _attack;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _aim;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _dash;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _lightningSelect;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _fireSelect;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _bloodSelect;
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _rangeAttack;

	UPROPERTY(EditAnywhere, Category=Input) TSoftObjectPtr<UInputMappingContext> _defaultInputMapping;

	
	
	UPROPERTY(VisibleAnywhere) USpringArmComponent* _cameraArmComponent    = nullptr;
	UPROPERTY(VisibleAnywhere) UBoxComponent*		_meleeHitbox		   = nullptr;
	UPROPERTY(VisibleAnywhere) USphereComponent*	_dashHitbox			   = nullptr;

	UPROPERTY(EditAnywhere, Category="Melee Stats") float _meleeCooldown		  = 3.f;
	UPROPERTY(EditAnywhere, Category="Melee Stats") int   _lightAttackMeleeDamage = 10.f;
	UPROPERTY(EditAnywhere, Category="Melee Stats") int   _heavyAttackMeleeDamage = 20.f;
	//how long the attack must be charged before it becomes heavy
	UPROPERTY(EditAnywhere, Category="Melee Stats") float _heavyAttackMeleeTime	  = 0.5f;

	float _meleeCooldownTimer = 0.f;
	float _meleeHeavyTimer    = 0.f;
	bool  _chargingAttack	  = false;

	UPROPERTY(EditAnywhere, Category="Ranged Stats") float				  _rangeCooldown = 3.f;
	

	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashDamage		= 5;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 float _dashDuration	= 0.2f;
	//UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashLength		= 1000;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashSpeed		= 10000;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashKnockBack	= 100;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashSelfDamage	= 0;

	

	UPROPERTY(EditDefaultsOnly, Category="projectile") TSubclassOf<AProjectileBaseClass> _electricProjectile;
	UPROPERTY(EditDefaultsOnly, Category="projectile") TSubclassOf<AProjectileBaseClass> _fireProjectile;
	UPROPERTY(EditDefaultsOnly, Category="projectile") TSubclassOf<AProjectileBaseClass> _bloodProjectile = AProjectileBaseClass::StaticClass();
	UPROPERTY(EditDefaultsOnly, Category="projectile") int								 _fireSelfDamage	 = 0;
	UPROPERTY(EditDefaultsOnly, Category="projectile") int								 _ligtningSelfDamage = 0;
	UPROPERTY(EditDefaultsOnly, Category="projectile") int								 _bloodSelfDamage = 0;

	bool _useElectric = true;
	int chosenAttack = 0;
	

	TArray<AProjectileBaseClass*> _pooledElectricProjectiles;
	int							  _electricProjectileToUse;
	TArray<AProjectileBaseClass*> _pooledFireProjectiles;
	int							  _fireProjectileToUse;

	UPROPERTY()AProjectileBaseClass* _bloodProjectileToUse;
	

	FVector		 _dashDirection  = FVector::Zero();
	FTimerHandle _dashHandle;

	//this is a failsafe to prevent actors from being hit multiple times
	UPROPERTY()TSet<AActor*> _dashHitActors;

	float _damageTakenStorage = 1;
	
	UFUNCTION()
	void HandleDashHits(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndexbool, bool bFromSweep,const FHitResult& SweepResult);

	//curve for the dash easing
	float easeInOutQuint(float x);
	
	float _rangeCooldownTimer = 0.f;

	bool _aiming = false;

	FVector _inputVector = FVector::Zero();
	void ResetInputVector(const FInputActionValue& Value);
	
	void MoveForwards(float val);
	void MoveSideways(float val);
	void LookRight	 (float val);
	void LookUp		 (float val);

	void LookAction       (const FInputActionValue& Value);
	void MoveAction       (const FInputActionValue& Value);
	void JumpAction		  (const FInputActionValue& Value);
	void MeleeAction	  (const FInputActionValue& Value);
	void BeginMeleeAction (const FInputActionValue& Value);
	void RangeAttackAction(const FInputActionValue& Value);
	void BeginAimAction	  (const FInputActionValue& Value);
	void StopAimAction	  (const FInputActionValue& Value);
	void DashAction		  (const FInputActionValue& Value);
	void FireAction		  (const FInputActionValue& Value);
	void lightningAction  (const FInputActionValue& Value);
	void BloodAction	  (const FInputActionValue& Value);

	void ResetDash		  ();

public:
	UFUNCTION(BlueprintImplementableEvent) void MeleeAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void HeavyMeleeAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void BeginMeleeAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void RangedAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void BeginAimEvent();
	UFUNCTION(BlueprintImplementableEvent) void StopAimEvent();
	UFUNCTION(BlueprintImplementableEvent) void DashEvent();
	UFUNCTION(BlueprintImplementableEvent) void FireConeEvent();

	UPROPERTY(VisibleAnywhere, Category="Dash Stats")bool CurrentlyDashing = false;

	void BeginPlay()				   override;
	void Tick	  (float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability Level") int _fireLevel		= 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability Level") int _electricLevel = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability Level") int _bloodLevel    = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ability Level") int _playerLevel   = 0;

	UPROPERTY(EditDefaultsOnly, Category=Upgrade) TArray<FPlayerUpgrades> Upgrades;
	UFUNCTION(BlueprintCallable) void UpgradePlayer();

		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) UCameraComponent*	_playerCameraComponent = nullptr;
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};