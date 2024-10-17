// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class ALevelGenerator;
class ABaseEnemyClass;
class ASpawnArea;
class UChaosManager;
class UNavigationSystemV1;

USTRUCT()
struct FEnemyGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TSubclassOf<ABaseEnemyClass>	EnemyClass;
	UPROPERTY(EditAnywhere) int32							Count;
	UPROPERTY(EditAnywhere) ASpawnArea*						SpawnArea;

	UPROPERTY(EditAnywhere) bool	OverrideDepthScalingFactor	= false;
	/** Decides at what rate the current room depth scales the enemy count.
	 * Final enemy count is calculated as count + count * room depth * depth scaling factor,
	 * where room depth starts at 0.
	 *
	 * Only used if OverrideDepthScalingFactor is true. */
	UPROPERTY(EditAnywhere) float	DepthScalingFactor			= 0.0f;
};

USTRUCT()
struct FEnemyWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TArray<FEnemyGroup> EnemyGroups;
	UPROPERTY(EditAnywhere) float				Duration;
};

UCLASS()
class GP4_TEAM05_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TArray<FEnemyWave> _waves = {};

	/** Decides at what rate the current room depth scales the enemy count.
	 * Final enemy count is calculated as count + count * room depth * depth scaling factor,
	 * where room depth starts at 0.
	 *
	 * May be overridden by individual enemy groups. */
	UPROPERTY(EditAnywhere) float _depthScalingFactor = 0.0f;
	
	int32			_currentWaveIndex = -1;
	FTimerHandle	_waveTimer;

	UPROPERTY() ACharacter*				_player			= nullptr;
	UPROPERTY() UChaosManager*			_chaosManager	= nullptr;
	UPROPERTY() ALevelGenerator*		_levelGenerator	= nullptr;
	UPROPERTY() UNavigationSystemV1*	_navSys			= nullptr;

	TMap<TSubclassOf<ABaseEnemyClass>, TSet<ABaseEnemyClass*>> _enemyPools;

	UPROPERTY() USceneComponent* _root = nullptr;
	
	FActorSpawnParameters spawnParams;
	
	UFUNCTION(BlueprintCallable) void SpawnNextWave();

	void SpawnEnemy
	(
		ABaseEnemyClass* enemy,
		const FVector& spawnPoint = FVector::ZeroVector
	);
	void SpawnNextWave();
	
	void DespawnEnemy(ABaseEnemyClass* enemy);
	
	void PrepareEnemy(FEnemyGroup& group);
	void PrepareEnemies();
	
	int ApplyRoomDepthMultiplier(int count, float depthScalingFactor ) const;
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	friend class ABaseEnemyClass;
};
