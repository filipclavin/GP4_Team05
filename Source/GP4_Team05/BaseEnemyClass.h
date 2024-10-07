// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacter.h"
#include "BaseEnemyClass.generated.h"

class AAIController;
class UBoxComponent;
class AEnemySpawner;
/**
 * 
 */
UCLASS()
class GP4_TEAM05_API ABaseEnemyClass : public AAuraCharacter
{
	GENERATED_BODY()

	ABaseEnemyClass();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void OnSpawned(AEnemySpawner* spawner);
protected:
	UPROPERTY(VisibleAnywhere) UBoxComponent* _meleeHitbox = nullptr;

	UPROPERTY(EditAnywhere, Category="attack stats")	 float _attackInterval = 0.5f;
	UPROPERTY(EditAnywhere, Category="attack stats")	 int   _attackDamage   = 40;
	UPROPERTY(EditDefaultsOnly, Category="attack stats") float _attackRange	   = 40;
	
	float _attackTimer;
	bool _inAttackRange;

	bool dead = false;

	UPROPERTY() AAIController* _controller = nullptr;
	UPROPERTY() AEnemySpawner* _spawner = nullptr;

	UPROPERTY(EditAnywhere) TMap<float, float> _distanceTickIntervals = {
		{0, 0.1f},
		{1000, 0.2f},
		{2000, 0.3f},
		{3000, 0.4f},
		{4000, 0.5f}
	};
	TArray<float> _thresholds;

	UPROPERTY() ACharacter* _playerCharacter = nullptr;

	UFUNCTION(BlueprintCallable) void MeleeAttack			();
	//incase you want to add extra logic to move orders
	UFUNCTION(BlueprintCallable) void EnemyMoveToLocation	(FVector location);
	//incase you want to add extra logic to move orders
	UFUNCTION(BlueprintCallable) void EnemyMoveToActor		(AActor* otherActor);
	//returns if the actor is within attack range
	UFUNCTION(BlueprintCallable) bool IsWithinRange			(AActor* otherActor);
	UFUNCTION(BlueprintCallable) void incrementTimerCounter (float deltatime);
	UFUNCTION(BlueprintCallable) void Die					();
	
	void UpdateTickInterval();

	friend class AEnemySpawner;
};
