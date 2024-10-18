// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyClass.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "ChaosManager.h"
#include "BloodPuddle.h"
#include "EnemySpawner.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Spawned, "Spawned")
UE_DEFINE_GAMEPLAY_TAG(TAG_Alive, "Alive")

ABaseEnemyClass::ABaseEnemyClass()
{
	_meleeHitbox = CreateDefaultSubobject<UBoxComponent>("melee hitbox");
	_meleeHitbox->SetupAttachment(RootComponent);
}

void ABaseEnemyClass::BeginPlay()
{
	Super::BeginPlay();

	_controller = Cast<AAIController>(GetController());

	if (_controller == nullptr){Destroy();}

	_distanceTickIntervals.KeySort(
		[](const float A, const float B) -> bool
		{
			return A < B;
		}
	);
	_distanceTickIntervals.GetKeys(_thresholds);
	
	_playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void ABaseEnemyClass::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateAuras(DeltaSeconds);
	UpdateTickInterval();

	if (!_combinedStats->_isAlive && !dead)
	{
		//this is a very stupid implementation and should be changed later
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass<UChaosManager>()->enemyKilled(_chaosAmountOnDeath);
		dead = true;
	}
}

void ABaseEnemyClass::InitSpawned(AEnemySpawner* spawner)
{
	_spawner = spawner;
	_combinedStats->_currentHealth = _combinedStats->_maxHealth;
	_combinedStats->_isAlive = true;
	_tags.AddTag(TAG_Spawned);
	_tags.AddTag(TAG_Alive);
	OnSpawned();
}

void ABaseEnemyClass::MeleeAttack()
{
	if (_attackInterval > _attackTimer){return;}
	
	TArray<AActor*> ActorsInMelee;
	_meleeHitbox->GetOverlappingActors(ActorsInMelee);

	AActor* playerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	for (AActor* HitActor : ActorsInMelee)
	{
		if (HitActor == playerActor)
		{
			Cast<AAuraCharacter>(playerActor)->QueueDamage(_attackDamage, PHYSICAL);
			_attackTimer = 0;
		}
	}
}

void ABaseEnemyClass::EnemyMoveToLocation(FVector location)
{
	_controller->MoveToLocation(location);
}

bool ABaseEnemyClass::IsWithinRange(AActor* otherActor)
{
	
	
	if ((otherActor->GetActorLocation() - GetActorLocation()).Length() < _attackRange)
	{
		return true;
	}
	return false;
}

void ABaseEnemyClass::EnemyMoveToActor(AActor* otherActor)
{
	_controller->MoveToActor
	(
		otherActor,
		0.0f,
		false,
		true,
		false,
		nullptr,
		true
	);
}

void ABaseEnemyClass::incrementTimerCounter(float deltatime)
{
	_attackTimer += deltatime;
}

void ABaseEnemyClass::Die()
{
	_tags.RemoveTag(TAG_Alive);
	OnDeath();
	Super::Die();
	
	//FVector DeathLocation = GetActorLocation();
	//FRotator DeathRotation = GetActorRotation();
	//ABloodPuddle* BloodPuddle = ABloodPuddle::SpawnPuddle(DeathLocation, DeathRotation);
	//if (BloodPuddle)
	//{
	//	FString LocationString = DeathLocation.ToString();
	//	
	//	   if (GEngine)
	//		   GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Blood Puddle spawned at: %s"), *LocationString));
	//}
	//else
	//{
	//	if (GEngine)
	//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Failed to spawn Blood Puddle!)");
	//	
	//}
}

void ABaseEnemyClass::Despawn()
{
	_controller->StopMovement();
	_spawner->DespawnEnemy(this);
}

void ABaseEnemyClass::UpdateTickInterval()
{
	if (_playerCharacter == nullptr) return;

	for (int i = 1; i < _thresholds.Num(); i++)
	{
		if (GetDistanceTo(_playerCharacter) < _thresholds[i])
		{
			float interval = _distanceTickIntervals[_thresholds[i-1]];
			if (GetActorTickInterval() == interval) return;
			
			SetActorTickInterval(interval);
			//_controller->SetActorTickInterval(interval);
			//for (UActorComponent* component : GetComponents())
			//{
			//	component->SetComponentTickInterval(interval);
			//}

			return;
		}
	}
}
