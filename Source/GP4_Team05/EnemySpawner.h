// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class ABaseEnemyClass;
class ASpawnArea;
class UNavigationSystemV1;

USTRUCT()
struct FEnemyGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) TSubclassOf<ABaseEnemyClass>	EnemyClass;
	UPROPERTY(EditAnywhere) int32							Count;
	UPROPERTY(EditAnywhere) ASpawnArea*						SpawnArea;
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
	
	int32			_currentWaveIndex = -1;
	FTimerHandle	_waveTimer;

	UPROPERTY() ACharacter*				_player = nullptr;
	UPROPERTY() UNavigationSystemV1*	_navSys = nullptr;

	TMap<TSubclassOf<ABaseEnemyClass>, TSet<ABaseEnemyClass*>> _enemyPools;
	
	void SpawnNextWave();
	void SpawnEnemy(ABaseEnemyClass* enemy, const FVector& spawnPoint = FVector::ZeroVector);
	void DespawnEnemy(ABaseEnemyClass* enemy);
	
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
