// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "NativeGameplayTags.h"
#include "BaseEnemyClass.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Spawned)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Alive)

class AAIController;
class UBoxComponent;
class AEnemySpawner;
class UDamageDisplay;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class GP4_TEAM05_API ABaseEnemyClass : public AAuraCharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = _tags; }

public:
	ABaseEnemyClass();
	
protected:
	UPROPERTY(VisibleAnywhere) UBoxComponent* _meleeHitbox = nullptr;

	UPROPERTY(EditAnywhere, Category="attack stats")	 float _attackInterval     = 0.5f;
	UPROPERTY(EditAnywhere, Category="attack stats")	 int   _attackDamage       = 40;
	UPROPERTY(EditDefaultsOnly, Category="attack stats") float _attackRange	       = 40;
	UPROPERTY(EditAnywhere)								 float _chaosAmountOnDeath = 20.0f;
	
	float _attackTimer;
	bool _inAttackRange;


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

	UPROPERTY() FGameplayTagContainer _tags;

	UFUNCTION(BlueprintCallable) void MeleeAttack			();
	//incase you want to add extra logic to move orders
	UFUNCTION(BlueprintCallable) void EnemyMoveToLocation	(FVector location);
	//incase you want to add extra logic to move orders
	UFUNCTION(BlueprintCallable) void EnemyMoveToActor		(AActor* otherActor);
	//returns if the actor is within attack range
	UFUNCTION(BlueprintCallable) bool IsWithinRange			(AActor* otherActor);
	UFUNCTION(BlueprintCallable) void incrementTimerCounter (float deltatime);
	
	UFUNCTION(BlueprintNativeEvent) void OnSpawned();
	void OnSpawned_Implementation() {}
	void InitSpawned(AEnemySpawner* spawner);
	
	UFUNCTION(BlueprintNativeEvent) void OnDeath();
	void OnDeath_Implementation() {}
	virtual void Die() override;
	UFUNCTION(BlueprintCallable) void Despawn();
	
	void UpdateTickInterval();

	UPROPERTY(BlueprintReadWrite) UDamageDisplay*   _damageDisplayWidget;
	UPROPERTY(EditAnywhere)       UWidgetComponent* _widget;

	friend class AEnemySpawner;
};
