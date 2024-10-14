// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "BaseEnemyClass.h"
#include "SpawnArea.h"
#include "ChaosManager.h"
#include "LevelGenerator.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"

void AEnemySpawner::SpawnNextWave()
{
	_currentWaveIndex++;

	for (FEnemyGroup& group : _waves[_currentWaveIndex].EnemyGroups)
	{
		if (group.Count == 0) continue;

		int finalCount = ApplyRoomDepthMultiplier(group.Count);
		
		TSet<ABaseEnemyClass*>& pool = _enemyPools[group.EnemyClass];

		FBoxSphereBounds bounds = group.SpawnArea->GetBounds();

		float boxAspectRatio = bounds.BoxExtent.X / bounds.BoxExtent.Y;
		int cols = FMath::Max(FMath::Floor(FMath::Sqrt(finalCount * boxAspectRatio)), 1);
		int rows = FMath::Floor(static_cast<float>(finalCount) / cols);

		while (cols * rows != finalCount)
		{
			float gridAspectRatio = cols / rows;

			if (cols * rows < finalCount)
			{
				gridAspectRatio > boxAspectRatio ? rows++ : cols++;
			}
			else if (cols * rows > finalCount)
			{
				if ((cols - 1) * rows < finalCount && cols * (rows - 1) < finalCount)
				{
					break;
				}
				
				gridAspectRatio > boxAspectRatio ? cols-- : rows--;
			}
		}

		TSet<ABaseEnemyClass*> toRemoveFromPool;

		while (pool.Num() < finalCount)
		{
			PrepareEnemy(group);
		}
		
		uint16 count = 0;
		for (ABaseEnemyClass* enemy : pool)
		{
			FVector spawnPoint = bounds.Origin +
				FVector(
					cols == 1 ? 0 : (count % cols) * bounds.BoxExtent.X * 2 / (cols - 1) - bounds.BoxExtent.X,
					rows == 1 ? 0 : (count / cols) * bounds.BoxExtent.Y * 2 / (rows - 1) - bounds.BoxExtent.Y,
					0
				);
			count++;
			
			SpawnEnemy(enemy, spawnPoint);
			toRemoveFromPool.Add(enemy);

			if (count == finalCount) break;
		}

		pool = pool.Difference(toRemoveFromPool);
		
	}

	if (_chaosManager->_chaosFull) return;

	if (_currentWaveIndex == _waves.Num()) _currentWaveIndex = -1;
	
	GetWorld()->GetTimerManager().SetTimer(
		_waveTimer,
		this,
		&AEnemySpawner::SpawnNextWave,
		_waves[_currentWaveIndex].Duration,
		false
	);
}

void AEnemySpawner::SpawnEnemy(ABaseEnemyClass* enemy, const FVector& spawnPoint)
{
	enemy->SetActorEnableCollision(true);
	enemy->SetActorLocation
	(
		spawnPoint,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
	enemy->SetActorHiddenInGame(false);
	enemy->SetActorTickEnabled(true);
	enemy->OnSpawned(this);
}

void AEnemySpawner::DespawnEnemy(ABaseEnemyClass* enemy)
{
	enemy->SetActorHiddenInGame(true);
	enemy->SetActorEnableCollision(false);
	enemy->SetActorTickEnabled(false);
	
	_enemyPools[enemy->GetClass()].Add(enemy);
}

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::PrepareEnemy(FEnemyGroup& group)
{
	ABaseEnemyClass* enemy = GetWorld()->SpawnActor<ABaseEnemyClass>(
		group.EnemyClass,
		FVector(0, 0, -10000),
		FRotator::ZeroRotator,
		spawnParams
	);
				
	enemy->SetActorHiddenInGame(true);
	enemy->SetActorEnableCollision(false);
	enemy->SetActorTickEnabled(false);

	if (!_enemyPools.Contains(group.EnemyClass))
	{
		_enemyPools.Add(group.EnemyClass, TSet<ABaseEnemyClass*>());
	}
				
	_enemyPools[group.EnemyClass].Add(enemy);
}

void AEnemySpawner::PrepareEnemies()
{
	for (FEnemyWave& wave : _waves)
	{
		for (FEnemyGroup& group : wave.EnemyGroups)
		{
			for (int i = 0; i < ApplyRoomDepthMultiplier(group.Count); i++)
			{
				PrepareEnemy(group);
			}
		}
	}
}

int AEnemySpawner::ApplyRoomDepthMultiplier(int count) const
{
	return count + count * _levelGenerator->GetRoomDepth() * DepthScalingFactor;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	_player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	_chaosManager = _player->FindComponentByClass<UChaosManager>();
	_levelGenerator = Cast<ALevelGenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()));
	_navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_player);

	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//PrepareEnemies();

	if (_waves.Num() > 0)
		SpawnNextWave();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

