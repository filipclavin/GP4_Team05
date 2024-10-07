// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "BaseEnemyClass.h"
#include "SpawnArea.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"

void AEnemySpawner::SpawnNextWave()
{
	_currentWaveIndex++;

	for (FEnemyGroup& group : _waves[_currentWaveIndex].EnemyGroups)
	{
		if (group.Count == 0) continue;
		
		TSet<ABaseEnemyClass*>& pool = _enemyPools[group.EnemyClass];

		FBoxSphereBounds bounds = group.SpawnArea->GetBounds();

		float boxAspectRatio = bounds.BoxExtent.X / bounds.BoxExtent.Y;
		int cols = FMath::Max(FMath::Floor(FMath::Sqrt(group.Count * boxAspectRatio)), 1);
		int rows = FMath::Floor(static_cast<float>(group.Count) / cols);

		while (cols * rows != group.Count)
		{
			float gridAspectRatio = cols / rows;

			if (cols * rows < group.Count)
			{
				gridAspectRatio > boxAspectRatio ? rows++ : cols++;
			}
			else if (cols * rows > group.Count)
			{
				if ((cols - 1) * rows < group.Count && cols * (rows - 1) < group.Count)
				{
					break;
				}
				
				gridAspectRatio > boxAspectRatio ? cols-- : rows--;
			}
		}

		TSet<ABaseEnemyClass*> toRemoveFromPool;
		
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

			if (count == group.Count) break;
		}

		pool = pool.Difference(toRemoveFromPool);
		
	}
	
	if (_currentWaveIndex == _waves.Num()) return;
	
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

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	_player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	_navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_player);

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FEnemyWave& wave : _waves)
	{
		for (FEnemyGroup& group : wave.EnemyGroups)
		{
			for (int i = 0; i < group.Count; i++)
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
		}
	}
	
	SpawnNextWave();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

