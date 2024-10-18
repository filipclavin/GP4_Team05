// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBaseClass.h"

#include "AuraCharacter.h"
#include "AuraHandler.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ChaosManager.h"

// Sets default values
AProjectileBaseClass::AProjectileBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_projectileCollider = CreateDefaultSubobject<USphereComponent>("collider");
	RootComponent = _projectileCollider;
}

// Called when the game starts or when spawned
void AProjectileBaseClass::BeginPlay()
{
	Super::BeginPlay();
	_projectileCollider->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBaseClass::HitEnemies);

	AActor* AuraHandlerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAuraHandler::StaticClass());

	if (AuraHandlerActor)
	{
		_handler = Cast<AAuraHandler>(AuraHandlerActor);
	}
}

// Called every frame
void AProjectileBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + GetActorForwardVector()*_projectileSpeed*DeltaTime, true);

	if (FVector::Distance(GetActorLocation(), _startLocation) > _projectileRange)
	{
		DespawnProjectile();
	}
}

void AProjectileBaseClass::SpawnProjectile(int upgradeAmount, APlayerCharacter* owningPlayer)
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	
	int additionalSpeed			= 0;
	int additionalRange			= 0;
	int additionalDamage		= 0;
	int additionalForkAmount	= 0;
	int additionalExplodeRadius = 0;
	int additionalHitEnemiesCap = 0;
	_upgradeAmount = upgradeAmount;

	for (int i = 0; i < upgradeAmount; i++)
	{
		if (upgradeStatIncreases.Num() > i)
		{	
			additionalSpeed			+= upgradeStatIncreases[i].additionalSpeed;
			additionalRange			+= upgradeStatIncreases[i].additionalRange;
			additionalDamage		+= upgradeStatIncreases[i].additionalDamage;
			additionalForkAmount	+= upgradeStatIncreases[i].additionalForkAmount;
			additionalExplodeRadius += upgradeStatIncreases[i].additionalExplosionRadius;
			additionalHitEnemiesCap += upgradeStatIncreases[i].additionalHitEnemiesCap;
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "Upgraded");
		}
	}
	
	_startLocation			   = GetActorLocation();
	_projectileSpeed		   = _baseProjectileSpeed			+ additionalSpeed;
	_projectileRange    	   = _baseProjectileRange			+ additionalRange;
	_projectileDamage		   = _baseProjectileDamage			+ additionalDamage;
	_projectileForking		   = _baseProjectileForking			+ additionalForkAmount;
	_projectileExplosionRadius = _baseProjectileExplosionRadius + additionalExplodeRadius;
	_hitEnemiesCap			   = _baseHitEnemiesCap				+ additionalHitEnemiesCap;

	if (owningPlayer != nullptr)
	{
		_owningPlayer = owningPlayer;
		if (_owningPlayer != nullptr)
		{
			_chaosManager = _owningPlayer->GetComponentByClass<UChaosManager>();
		}
	}
}

void AProjectileBaseClass::DespawnProjectile()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void AProjectileBaseClass::HitEnemies(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndexbool, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)){return;}

	if (!OtherActor->IsA<AAuraCharacter>()) {DespawnProjectile();return;}
	
	TArray<AActor*>	lightningHits;
	TArray<AActor*>	alreadyHitActors;
	TQueue<AActor*> ActorsToProcess;

	UClass* AuraCharacterClass = AAuraCharacter::StaticClass();
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	
	AAuraCharacter* hitCharacter = Cast<AAuraCharacter>(OtherActor);
		
	alreadyHitActors.Add(hitCharacter);
	ActorsToProcess.Enqueue(hitCharacter);

	int numberOfForks = 0;

	DespawnProjectile();
	
	while (!ActorsToProcess.IsEmpty())
	{
		if (numberOfForks >= _projectileForking)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "limit Reached");
				break;
		}
		lightningHits.Empty();
		AActor* Actor;
		ActorsToProcess.Dequeue(Actor);
		
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange,
		traceObjectTypes, AuraCharacterClass, alreadyHitActors, lightningHits);
		DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), _baseProjectileForkingRange, 12, FColor::Red, true, 4.0f);

		for (AActor* hitActor : lightningHits)
		{
			DrawDebugLine(GetWorld(), Actor->GetActorLocation(), hitActor->GetActorLocation(), FColor::Yellow, true, 4, 0, 1);
			alreadyHitActors.Add(hitActor);
			ActorsToProcess.Enqueue(hitActor);
			numberOfForks++;
		}
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, FString::FromInt(numberOfForks) +" of " +FString::FromInt(_projectileForking));

	DealDamage(alreadyHitActors);
	
	
}

void AProjectileBaseClass::DealDamage(TArray<AActor*> hitCharacter)
{
	int numberOfForks = 0;
	
	for (AActor* hitActor : hitCharacter)
    	{
    		
    		if (hitActor->IsA<AAuraCharacter>())
    		{
    			//TODO change to lightning when its ready
    			AAuraCharacter* hitCharacter = Cast<AAuraCharacter>(hitActor);
    			hitCharacter->QueueDamage(_projectileDamage, ElementTypes::LIGHTNING);
    			_owningPlayer->UpdateAurasOnAttackHits(hitCharacter, LIGHTNING_ATTACK);
    			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, hitActor->GetName() + " hit");
    			numberOfForks++;
    			
    			
    			if (numberOfForks >= _projectileForking)
    			{
    			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "limit Reached");
    			break;
    			}
    		}
    			
    	}
}
