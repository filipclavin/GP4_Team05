// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBaseClass.h"

#include "AuraCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

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

void AProjectileBaseClass::SpawnProjectile(int additionalSpeed, int additionalRange, int additionalForkAmount, int additionalDamage)
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	
	_startLocation	   = GetActorLocation();
	_projectileSpeed   = _baseProjectileSpeed   + additionalSpeed;
	_projectileRange   = _baseProjectileRange   + additionalRange;
	_projectileDamage  = _baseProjectileDamage  + additionalDamage;
	_projectileForking = _baseProjectileForking + additionalForkAmount;
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

	numberOfForks = 0;
	
	for (AActor* hitActor : alreadyHitActors)
	{
		
		if (hitActor != this)
		{
			if (numberOfForks >= _projectileForking)
			{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, "limit Reached");
			break;
			}
			
			//TODO change to lightning when its ready
			Cast<AAuraCharacter>(hitActor)->QueueDamage(_projectileDamage, ElementTypes::WATER);
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow, hitActor->GetName() + " hit");
			numberOfForks++;
		}
			
	}
	
}
