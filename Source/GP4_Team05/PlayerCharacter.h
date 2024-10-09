// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacter.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USphereComponent;
class USpringArmComponent;
class UBoxComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

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
	UPROPERTY(EditAnywhere, Category=input) UInputAction* _fireCone;

	UPROPERTY(EditAnywhere, Category=Input) TSoftObjectPtr<UInputMappingContext> _defaultInputMapping;

	
	UPROPERTY(VisibleAnywhere) UCameraComponent*	_playerCameraComponent = nullptr;
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
	UPROPERTY(EditAnywhere, Category="Ranged Stats") TSubclassOf<AActor> _projectilePrefab;

	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashDamage		= 5;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 float _dashDuration	= 0.2f;
	//UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashLength		= 1000;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashSpeed		= 10000;
	UPROPERTY(EditAnywhere, Category="Dash Stats")	 int   _dashKnockBack	= 100;

	UPROPERTY(EditAnywhere, Category="Arcing Surge") int   _Ligtningdamage	= 10;
	UPROPERTY(EditAnywhere, Category="Arcing Surge") int   _spreadRadius	= 100;
	UPROPERTY(EditAnywhere, Category="Arcing Surge") int   _range			= 3000;

	
	UPROPERTY(EditAnywhere, Category="Inferno Cascade") int _widthOfFireCone = 30;
	UPROPERTY(EditAnywhere, Category="Inferno Cascade") int _rangeOfFireCone = 400;

	float _widthOfFireConeRadians = _widthOfFireCone/(180.0/3.141592653589793238463);
	
	//FVector		 _dashTargetLocation = FVector::Zero();
	//FVector		 _dashStartLocation  = FVector::Zero();
	FVector		 _dashDirection  = FVector::Zero();
	FTimerHandle _dashHandle;

	//this is a failsafe to prevent actors from being hit multiple times
	UPROPERTY()TSet<AActor*> _dashHitActors;

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
	void FireConeAction	  (const FInputActionValue& Value);

	void ResetDash		  ();

	
public:
	UFUNCTION(BlueprintImplementableEvent) void MeleeAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void BeginMeleeAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void RangedAttackEvent();
	UFUNCTION(BlueprintImplementableEvent) void BeginAimEvent();
	UFUNCTION(BlueprintImplementableEvent) void StopAimEvent();
	UFUNCTION(BlueprintImplementableEvent) void DashEvent(FVector targetLocation);
	UFUNCTION(BlueprintImplementableEvent) void FireConeEvent();

	UPROPERTY(VisibleAnywhere, Category="Dash Stats")bool CurrentlyDashing = false;

	void BeginPlay()				   override;
	void Tick	  (float DeltaSeconds) override;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};