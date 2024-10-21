// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "AIController.h"
#include "BaseEnemyClass.h"
#include "SpawnArea.h"
#include "ChaosManager.h"
#include "LevelGenerator.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

void AEnemySpawner::SpawnNextWave()
{
	if (_chaosManager->_chaosFull)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chaos is full, not spawning any more enemies!"));
		return;
	}

	_currentWaveIndex++;

	for (FEnemyGroup& group : _waves[_currentWaveIndex].EnemyGroups)
	{
		if (group.Count == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Enemy group count is 0!"));
			continue;
		}

		if (!group.EnemyClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Enemy group has no enemy class assigned!"));
			continue;
		}

		if (!group.SpawnArea)
		{
			UE_LOG(LogTemp, Error, TEXT("Enemy group has no spawn area assigned!"));
			continue;
		}
		
		TSet<ABaseEnemyClass*>& pool = _enemyPools[group.EnemyClass];

		if (pool.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No enemies in pool for %s, skipping this group..."), *group.EnemyClass->GetName());
			continue;
		}
			
		uint16 finalCount = ApplyRoomDepthMultiplier
		(
			group.Count,
			group.OverrideDepthScalingFactor ? group.DepthScalingFactor : _depthScalingFactor
		);
		UE_LOG(LogTemp, Warning, TEXT("Trying to spawn group of %d %ss"), finalCount, *group.EnemyClass->GetName());
		
		if (pool.Num() < finalCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not enough enemies in pool, only %d available!"), pool.Num());
			finalCount = pool.Num();
		}

		FBoxSphereBounds bounds = group.SpawnArea->GetBounds();

		float boxAspectRatio = bounds.BoxExtent.X / bounds.BoxExtent.Y;
		int cols = FMath::Max(FMath::Floor(FMath::Sqrt(finalCount * boxAspectRatio)), 1);
		int rows = FMath::Max(FMath::Floor(static_cast<float>(finalCount) / cols), 1);

		while (cols * rows != finalCount)
		{
			float gridAspectRatio = cols / rows;

			if (cols * rows < finalCount)
			{
				gridAspectRatio > boxAspectRatio ? rows++ : cols++;
			}
			else if (cols * rows > finalCount)
			{
				if (gridAspectRatio > boxAspectRatio)
				{
					if ((cols - 1) * rows < finalCount) break;
					cols--;
				}
				else
				{
					if ((rows - 1) * cols < finalCount) break;
					rows--;
				}
			}
		}

		TSet<ABaseEnemyClass*> toRemoveFromPool;
		
		uint16 numSpawned = 0;
		for (ABaseEnemyClass* enemy : pool)
		{
			FVector spawnPoint = bounds.Origin +
				FVector(
					cols == 1 ? 0 : (numSpawned % cols) * bounds.BoxExtent.X * 2 / (cols - 1) - bounds.BoxExtent.X,
					rows == 1 ? 0 : (numSpawned / cols) * bounds.BoxExtent.Y * 2 / (rows - 1) - bounds.BoxExtent.Y,
					0
				);
			
			SpawnEnemy(enemy, spawnPoint);
			toRemoveFromPool.Add(enemy);

			numSpawned++;
			if (numSpawned == finalCount) break;
		}

		UE_LOG(LogTemp, Warning, TEXT("Spawned %d %ss"), numSpawned, *group.EnemyClass->GetName());
		
		pool = pool.Difference(toRemoveFromPool);
		
	}

	float duration = _waves[_currentWaveIndex].Duration;
	
	if (_currentWaveIndex == _waves.Num() - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached final wave, looping back to first wave!"));
		_currentWaveIndex = -1;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		_waveTimer,
		this,
		&AEnemySpawner::SpawnNextWave,
		duration,
		false
	);
}

void AEnemySpawner::SpawnEnemy(ABaseEnemyClass* enemy, const FVector& spawnPoint)
{
	FNavLocation navLoc;
	_navSys->ProjectPointToNavigation(spawnPoint, navLoc, FVector(1000, 1000, 1000));
	
	enemy->SetActorEnableCollision(true);
	enemy->SetActorLocation
	(
		navLoc.Location + FVector(0, 0, enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
	enemy->SetActorHiddenInGame(false);
	enemy->SetActorTickEnabled(true);
	
	enemy->InitSpawned(this);
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

	_root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_root);
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
	enemy->_controller->UnPossess();

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
			int finalCount = ApplyRoomDepthMultiplier
			(
				group.Count,
				group.OverrideDepthScalingFactor ? group.DepthScalingFactor : _depthScalingFactor
			);
			for (int i = 0; i < finalCount; i++)
			{
				PrepareEnemy(group);
			}
		}
	}
}

int AEnemySpawner::ApplyRoomDepthMultiplier(int count, float depthScalingFactor) const
{
	if (_levelGenerator == nullptr) return count;
	
	return count + count * _levelGenerator->GetRoomDepth() * depthScalingFactor;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	_player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	_chaosManager = _player->FindComponentByClass<UChaosManager>();
	if (AActor* levelGenerator = UGameplayStatics::GetActorOfClass(GetWorld(), ALevelGenerator::StaticClass()))
	{
		_levelGenerator = Cast<ALevelGenerator>(levelGenerator);
	}
	_navSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(_player);

	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PrepareEnemies();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

