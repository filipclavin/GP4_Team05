// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyClass.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "ChaosManager.h"
#include "EnemySpawner.h"

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

	
	UpdateTickInterval();

	if (_combinedStats->_currentHealth <= 0 && !dead)
	{
		//this is a very stupid implementation and should be changed later
		UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetComponentByClass<UChaosManager>()->enemyKilled();
		dead = true;
	}
}

void ABaseEnemyClass::OnSpawned(AEnemySpawner* spawner)
{
	_spawner = spawner;

	EnemyMoveToActor(_playerCharacter);
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
			Cast<AAuraCharacter>(playerActor)->QueueDamage(_attackDamage, FIRE);
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
